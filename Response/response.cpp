#include "../HTTPServer.hpp"

void check_errors(Client &client, int code)
{
    std::stringstream ss;
    int fd;

    ss << code;
    std::map<std::string, std::string> errors = client.getServer().getErrorPage();
    fd = open(errors[ss.str()].c_str(), O_RDONLY);

    if (fd < 0)
    {
        check_errors(client, 500);
        return;
    }
    
    if (client.getResponse().getFileFd())
        close(client.getResponse().getFileFd());
    client.getResponse().setFileFd(fd);
}

location searchLocation(std::vector<location> locations, std::string _location)
{
    location def;

    for (size_t i = 0; i < locations.size(); i++)
    {
        if (locations[i].getPath() == _location)
            return locations[i];
        if (locations[i].getPath() == "/")
            def = locations[i];
    }
    return def;
}

void checkResourceExistence(const char *res, int &fd, bool &isDir, Client &client)
{
    DIR *dir;

    if (!access(res, F_OK))
    {
        if (!access(res, R_OK))
        {
            if (!(dir = opendir(res)))
                fd = open(res, O_RDONLY);
            else
            {
                closedir(dir);
                isDir = true;
            }
        }
        else
            client.setStatus(403);
    }
    else
        client.setStatus(404);
}

void getFile(Client &client, int s)
{
    int a;
    int fd;
    int max = 2048;
    char buff[max];

    if (client.getResponse().getFileFd() < 0)
    {
        check_errors(client, 500);
        client.setStatus(500);
    }

    fd = client.getResponse().getFileFd();

    if ((a = read(fd, buff, max)) > 0)
        client.getResponse().getBodySize() += a;

    if (!a)
    {
        lseek(fd, 0, SEEK_SET);

        if (s)
            client.setStatus(200);
        client.setState(BUILDING_2);
    }
    else if (a < 0)
    {
        check_errors(client, 500);
        client.setStatus(500);
    }
}

void getDir(Client &client, std::string src)
{
    std::vector<std::string> indexes;
    std::string file;
    std::string tmp;
    int fd;

    tmp = client.getRequest().getURI();
    indexes = client.getlocation().getIndex();
    if (tmp[tmp.length() - 1] != '/')
    {
        client.getResponse().setLocationUrl(client.getResponse().getOldUrl() + "/");
        client.setStatus(301);
        return;
    }
    else if (!indexes.empty())
    {
        for (size_t i = 0; i < indexes.size(); i++)
        {
            file = src + indexes[i];
            std::cout << "FILE: " << file << std::endl;
            if ((fd = open(file.c_str(), O_RDONLY)) > 0)
            {
                if (client.getlocation().isCgi())
                {
                    client.setState(WAITING_CGI);
                    run_cgi(client, file);
                }
                else
                {
                    client.getResponse().setFileFd(fd);
                    client.setState(FILE_READING);
                }
                return;
            }
        }
        client.setStatus(404);
    }
    std::cout << "AUTOINDEX: " << client.getlocation().getAutoIndex() << std::endl;
    if (client.getlocation().getAutoIndex().empty())
        client.setStatus(403);
    else
    {
        client.getResponse().setBody(createAutoindexPage(src));
        client.setState(BUILDING_2);
        client.setStatus(200);
    }
}

void get(Client &client, std::string src)
{
    bool isDir = false;
    int fd = 0;
    std::vector<std::string> allowed_methods = client.getlocation().getAllowMethodes();

    if (find(allowed_methods.begin(), allowed_methods.end(), "GET") == allowed_methods.end())
    {
        client.setStatus(405);
        return;
    }
    checkResourceExistence(src.c_str(), fd, isDir, client);
    if (fd > 0)
    {
        if (client.getlocation().isCgi())
        {
            client.setState(WAITING_CGI);
            run_cgi(client, src);
            return;
        }
        client.getResponse().setFileFd(fd);
        client.setState(FILE_READING);
    }
    else if (isDir)
        getDir(client, src);
}

void buildResponse(Client &client, std::string &response)
{
    std::stringstream ss;
    std::string crlf = "\r\n";

    if (client.getResponse().getBody().length())
        client.getResponse().setBodySize(client.getResponse().getBody().length());
    ss << client.getResponse().getBodySize();
    response = client.getResponse().getStatusLine(client.getStatus()) + crlf;
    response += "Server: " + client.getResponse().getServer() + crlf;
	// response += ; // ! cookies
	// response += ;
    if (client.getResponse().getLocationUrl().length())
        response += "Location: " + client.getResponse().getLocationUrl() + crlf;
    response += "Content-Length: " + ss.str() + crlf;

    response += crlf;
    if (client.getResponse().getBody().length())
    {
        response += client.getResponse().getBody();
        client.getResponse().setBodySize(0);
    }
}

void check_redirections(Client &client)
{
    location &_location = client.getlocation();

    if (_location.getReturn().length())
    {
        if (_location.getRedirection() == "location")
        {
            client.setlocation(searchLocation(client.getlocations(), client.getlocation().getReturn()));
            if (!client.getlocation().getPath().length())
                client.setStatus(404);
        }
        else
        {
            client.getResponse().setLocationUrl(_location.getReturn());
            client.setStatus(301);
        }
    }
}

void sendResponse(Client &client);

void sendCgi(Client &client);

void response(Client &client)
{
    std::string tmp;
    std::string src;
    std::string root;
    std::string &response = client.getResponse().getResponse();

    if (client.getState() == BUILDING)
    {
        client.getResponse().setOldUrl(client.getRequest().getURI());
        if (!client.getStatus())
            locationMatching(client.getRequest().getURI(), client);
        root = client.getlocation().getRoot();
        tmp = client.getRequest().getURI();

        if (tmp[0] == '/' && root[root.length() - 1] == '/' && tmp.length() > 1)
            tmp.erase(0, 1);

        src = root + tmp;

        std::cout << "SRC: " << src << std::endl;
        if (!client.getStatus())
            check_redirections(client);

        std::cout << "Method: " << client.getRequest().getMethod() << std::endl;

        if (!client.getStatus())
        {
            if (client.getRequest().getMethod() == "GET")
                get(client, src);
            else if (client.getRequest().getMethod() == "POST")
                Post(client.getRequest(), client.getlocation(), client);
            else if (client.getRequest().getMethod() == "DELETE") //==> check if delete is allowed.
                handleDeleteRequest(client, src);
        }
        if (client.getStatus() != 200 && client.getStatus())
        {
            std::cout << "ERROR: " << client.getStatus() << std::endl;
            client.setState(FILE_READING);
            check_errors(client, client.getStatus());
            client.err = 0;
        }
    }

    if (client.getState() == MOVING_FILE)
        writeToNewFile(client);

    if (client.getState() == FILE_READING)
        getFile(client, client.err);

    if (client.getState() == BUILDING_2)
    {
        buildResponse(client, response);
        if (client.getState() != WAITING_CGI)
            client.setState(SENDING);
    }

    if (client.getState() == WAITING_CGI)
    {
        if (time(0) - client.getStartTime() > 5)
        {
            std::cout << "CGI timed out" << std::endl;
            client.err = 0;
            client.setState(FILE_READING);
            client.setStatus(504);
            check_errors(client, 504);
            return;
        }
        pid_t pid = waitpid(client.getChildPid(), NULL, WNOHANG);
        if (pid == -1)
        {
            client.setStatus(500);
            check_errors(client, 500);
        }
        else if (pid)
        {
            client.setChildPid(0);
            lseek(client.getCgiFd(), 0, SEEK_SET);
            client.setState(SENDING_CGI);
        }
    }

    if (client.getState() == SENDING_CGI)
    {
        std::cout << "Sending CGI" << std::endl;
        sendCgi(client);
    }
    else if (client.getState() == SENDING)
        sendResponse(client);
}

// Check if the client close the connection before sending

void sendCgi(Client &client)
{
    size_t size = 2048;
    char buff[size];
    int sent;
    int a;
    int r;
    std::string status = "HTTP/1.1 200 OK\r\n";
    string header = "";
    int rd = 1;
    int readed = 0;

    while (rd > 0)
    {
        string content = "", left = "";
        memset(buff, 0, size);
        rd = read(client.getCgiFd(), buff, size - 1);
        if (rd)
        {
            if (rd < 0)
            {
                check_errors(client, 500);
                client.setStatus(500);
                return;
            }
            content += buff;
            if (content.find("Status: ") != string::npos && content.find("Status: ") < content.find("\r\n\r\n"))
            {
                string status_line = content.substr(content.find("Status: "));
                status_line.erase(status_line.find("\r\n") + 2);
                status = "HTTP/1.1 " + status_line.substr(status_line.find_first_of("0123456789"));
                content.erase(content.find("Status: "), status_line.length());
            }
            header += content;
            readed += rd;
            if (header.find("\r\n\r\n") != string::npos)
                break;
        }
    }
    header.insert(0, status);
    lseek(client.getCgiFd(), readed, SEEK_SET);
    std::cout << "*******\n"
              << header << std::endl;
    if ((sent = send(client.getClientSocket(), header.c_str(), header.size(), 0)) <= 0)
    {
        std::cout << "Client Closed the connection: " << std::endl;
        client.setState(DONE);
        return;
    }
    size -= sent;
    while (size--)
    {
        memset(buff, 0, size);
        r = read(client.getCgiFd(), buff, 2048);
        if (r <= 0)
            break;
        a = send(client.getClientSocket(), buff, r, 0);
        if (a <= 0)
        {
            std::cout << "Client Closed the connection: " << std::endl;
            client.setState(DONE);
            return;
        }
    }
    if (r < 0)
    {
        check_errors(client, 500);
        client.setStatus(500);
        return;
    }
    if (r && !client.getResponse().getBodySize())
        client.getResponse().getBodySize()++;
    if (!r || !client.getResponse().getBodySize())
        client.setState(DONE);
}

void sendResponse(Client &client)
{
    std::string response;
    size_t size = 2048;
    char buff[size];
    size_t i = 0;
    int a = 0;
    int r;

    if (client.getResponse().getResponse().length())
    {
        response = client.getResponse().getResponse();
        std::cout << "\n"
                  << response << std::endl;
        if (send(client.getClientSocket(), response.c_str(), response.size(), 0) < 0)
        {
            std::cout << "Client Closed the connection: " << std::endl;
            client.setState(DONE);
            return;
        }
        i = response.size();
        client.getResponse().setResponse("");
    }

    if (client.getResponse().getBodySize())
    {
        while (i++ < size && client.getResponse().getBodySize())
        {
            memset(buff, 0, size);
            if ((r = read(client.getResponse().getFileFd(), buff, size)) <= 0)
                break;
            // std::cout << c;
            a = send(client.getClientSocket(), buff, r, 0);
            if (a < 0)
            {
                std::cout << "Client Closed the connection: " << std::endl;
                client.setState(DONE);
                return;
            }
            i += a;
            client.getResponse().getBodySize() -= a;
        }
    }
    if (r < 0)
    {
        check_errors(client, 500);
        client.setStatus(500);
        return;
    }
    // if (r && !client.getResponse().getBodySize())
    //     client.getResponse().getBodySize()++;
    if (!r || !client.getResponse().getBodySize())
        client.setState(DONE);
}

// Fix the Status in CGI timeout