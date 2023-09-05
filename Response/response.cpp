#include "../HTTPServer.hpp"

void check_errors(Client &client, int code)
{
    std::stringstream ss;
    int fd;

    ss << code;
    std::map<std::string, std::string> errors = client.getServer().getErrorPage();
    if (code == 500)
        fd = client.getServer().getInternalErrPage();
    else
        fd = open(errors[ss.str()].c_str(), O_RDONLY);
    
    if (fd < 0)
    {
        client.setStatus(500);
        fd = client.getServer().getInternalErrPage();
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
    char *buff = new char[max + 1];

    if (client.getResponse().getFileFd() < 0)
    {
        check_errors(client, 500);
        client.setStatus(500);
    }

    fd = client.getResponse().getFileFd();

    if ((a = read(fd, buff, max)) > 0)
        client.getResponse().getBodySize() += a;

    delete[] buff;

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
        client.err = 0;
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
    // std::cout << "AUTOINDEX: " << client.getlocation().getAutoIndex() << std::endl;
    if (client.getlocation().getAutoIndex().empty())
        client.setStatus(403);
    else
    {
		// cout << YELLOW "SRC: " << src << WHITE << "\n";
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

void addCookies(Client client, std::string &response)
{
    std::map<std::string, std::string> cookies = client.getRequest().getCookies();
    std::map<std::string, std::string>::iterator it = cookies.begin();

    for (; it != cookies.end(); it++)
        response += "Set-Cookie: " + it->first + "=" + it->second + "\r\n";
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
    // add cookies
    addCookies(client, response);
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
        if (!client.getStatus())
            check_redirections(client);
        if (!client.getStatus())
        {
            if (client.getRequest().getMethod() == "GET")
                get(client, src);
            else if (client.getRequest().getMethod() == "POST")
                Post(client.getRequest(), client.getlocation(), client);
            else if (client.getRequest().getMethod() == "DELETE")
                handleDeleteRequest(client, src);
        }
        if (client.getStatus() != 200 && client.getStatus())
        {
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

void sendCgi(Client &client)
{
    std::string status = "HTTP/1.1 200 OK\r\n";
    std::string header = "";
    char        *buff = new char[2048];
    int         size = 2048;
    int         sent;
    int         a;
    int         r;
    int         rd = 1;
    int         readed = 0;

    while (rd > 0)
    {
        string content = "", left = "";
        memset(buff, 0, size);
        rd = read(client.getCgiFd(), buff, size - 1);
        if (rd < 0)
        {
            client.setStatus(500);
            client.setState(FILE_READING);
            check_errors(client, 500);
            client.err = 0;
            delete[] buff;
            return;
        }
        if (rd)
        {
            content += buff;
            if (content.find("Status: ") != string::npos && content.find("Status: ") < content.find("\r\n\r\n"))
            {
                string status_line = content.substr(content.find("Status: "));
                status_line.erase(status_line.find("\r\n") + 2);
                status = "HTTP/1.1 " + status_line.substr(status_line.find_first_of("0123456789"));
                content.erase(content.find("Status: "), status_line.length());
            }
            else if (content.find("HTTP/1.1 ") != string::npos && content.find("HTTP/1.1 ") < content.find("\r\n\r\n"))
            {
                string status_line = content.substr(content.find("HTTP/1.1 "));
                status_line.erase(status_line.find("\r\n") + 2);
                status = status_line;
                content.erase(content.find("HTTP/1.1 "), status_line.length());
            }
            header += content;
            readed += rd;
            if (header.find("\r\n\r\n") != string::npos)
                break;
        }
    }
    header.insert(0, status);
    lseek(client.getCgiFd(), readed, SEEK_SET);
    if ((sent = send(client.getClientSocket(), header.c_str(), header.size(), 0)) <= 0)
    {
        std::cout << "Client Closed the connection: " << std::endl;
        client.setState(DONE);
        delete[] buff;
        return;
    }
    size -= sent;
    while (--size > 0)
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
            delete[] buff;
            return;
        }
    }
    delete[] buff;
    if (r < 0)
    {
        client.setState(FILE_READING);
        client.err = 0;
        check_errors(client, 500);
        client.setStatus(500);
        return;
    }
    if (!r || !client.getResponse().getBodySize())
    {
		if (client.uploadedOutFile != -1)
			close(client.uploadedOutFile);
        client.setState(DONE);
    }
}

void sendResponse(Client &client)
{
    std::string     response;
    size_t          size = 2048;
    char            *buff = new char[size + 1];
    size_t          i = 0;
    int             a = 0;
    int             r;
    int             sent;

    if (client.getResponse().getResponse().length())
    {
        response = client.getResponse().getResponse();
        sent = send(client.getClientSocket(), response.c_str(), response.size(), 0);
		// cout << GREEN "SEND To " << client.getClientSocket() << WHITE "\n";
        if (sent <= 0)
        {
            std::cout << "Client Closed the connection: " << std::endl;
            client.setState(DONE);
            delete[] buff;
            return;
        }
            i = response.size();
            client.getResponse().setResponse("");
    }

    if (client.getResponse().getBodySize())
    {
        while (i < size && client.getResponse().getBodySize())
        {
            memset(buff, 0, size);
            if ((r = read(client.getResponse().getFileFd(), buff, size)) <= 0)
                break;
            a = send(client.getClientSocket(), buff, r, 0);
			// cout << GREEN "SEND To " << client.getClientSocket() << WHITE "\n";
            if (a <= 0)
            {
                std::cout << "Client Closed the connection: " << std::endl;
                client.setState(DONE);
                delete[] buff;
                return;
            }
            i += a;
            client.getResponse().getBodySize() -= a;
        }
    }
    delete[] buff;
    if (r < 0)
    {
        client.setState(FILE_READING);
        client.err = 0;
        check_errors(client, 500);
        client.setStatus(500);
        return;
    }
    if (!r || !client.getResponse().getBodySize())
    {
        if (client.getStatus() == 500)
            lseek(client.getResponse().getFileFd(), 0, SEEK_SET);
        client.setState(DONE);
    }
}
