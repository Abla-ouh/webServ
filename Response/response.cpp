#include "../HTTPServer.hpp"

void check_errors(Client &client, int code)
{
    std::stringstream ss;
    int fd;

    ss << code;
    std::map<std::string, std::string> errors = client.getServer().getErrorPage();
    fd = open(errors[ss.str()].c_str(), O_RDONLY);
    std::cout << "FD: " << fd << " " << errors[ss.str()] << std::endl;
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

void getFile(Client &client, bool s)
{
    char c;
    int max = 2048;
    int a;
    int fd;

    if (client.getResponse().getFileFd() < 0)
    {
        check_errors(client, 500);
        client.setStatus(500);
    }

    fd = client.getResponse().getFileFd();

    while ((a = read(fd, &c, 1)) > 0 && max--)
        client.getResponse().getBodySize()++;

    if (!a)
    {
        lseek(fd, 0, SEEK_SET);

        if (s)
            client.setStatus(200);
        client.setState(BUILDING_2);
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
    if (client.getlocation().getAutoIndex() == "off")
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
            std::cout << "code 1: " << client.getStatus() << std::endl;
            client.setState(WAITING_CGI);
            run_cgi(client, src);
            std::cout << "code 2: " << client.getStatus() << std::endl;
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
        locationMatching(client.getRequest().getURI(), client);
        root = client.getlocation().getRoot();
        tmp = client.getRequest().getURI();

        if (tmp[0] == '/' && root[root.length() - 1] == '/' && tmp.length() > 1)
            tmp.erase(0, 1);

        src = root + tmp;

        std::cout << "SRC: " << src << std::endl;
        if (!client.getStatus())
            check_redirections(client);

        if (!client.getStatus())
        {
            if (client.getRequest().getMethod() == "GET")
                get(client, src);
            else if (client.getRequest().getMethod() == "POST")
                Post(client.getRequest(), client.getlocation(), client);
            // else if (client.getRequest().getMethod() == "DELETE") ==> check if delete is allowed.
            //     handleDeleteRequest(client, src);
        
        }
        if (client.getStatus() != 200 && client.getStatus())
        {
            std::cout << "ERROR: " << client.getStatus() << std::endl;
            client.setState(FILE_READING);
            check_errors(client, client.getStatus());
            getFile(client, 0);
        }
    }
    
    if (client.getState() == FILE_READING)
        getFile(client, 1);

    if (client.getState() == BUILDING_2)
    {
        buildResponse(client, response);
        if (client.getState() != WAITING_CGI)
            client.setState(SENDING);
    }

    if (client.getState() == WAITING_CGI)
    {
        // Check if CGI timed out
        // if CGI has done, set state to SENDING. Else, close connection and set the appropriate status.
        std::cout << "Waiting CGI" << std::endl;
        pid_t pid = waitpid(client.getChildPid(), NULL, WNOHANG);
        if (pid == -1)
            client.setState(DONE);
            // client.setStatus(500);
        else if (pid)
        {
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
    char    c;
    int     a;
    int     r;
    std::string status = "HTTP/1.1 200 OK\r\n";

    send(client.getClientSocket(), status.c_str(), status.size(), 0);
    while (size--)
    {
        r = read(client.getCgiFd(), &c, 1);
        if (r <= 0)
        {
            std::cout << r << std::endl;
            break;
        }
        a = send(client.getClientSocket(), &c, 1, 0);
        std::cout << "Sent: " << c << std::endl;
        if (a < 0)
        {
            std::cout << "Client Closed the connection: " << std::endl;
            client.setState(DONE);
            return;
        } 
    }
    if (!r)
        client.setState(DONE);
}

void sendResponse(Client &client)
{
    size_t size = 2048;
    size_t     i = 0;
    int     a = 0;
    std::string response;
    char c;

    std::cout << "Sending response" << std::endl;
    if (client.getResponse().getResponse().length())
    {
        // std::cout << "Sending response header" << std::endl;
        response = client.getResponse().getResponse();
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
        // std::cout << "Sending response body" << std::endl;
        while (i++ < size && client.getResponse().getBodySize())
        {
            if (read(client.getResponse().getFileFd(), &c, 1) <= 0)
                break;
            a = send(client.getClientSocket(), &c, 1, 0);
            if (a < 0)
            {
                std::cout << "Client Closed the connection: " << std::endl;
                client.setState(DONE);
                return;
            }
            // else if (a < 1)
            // {
            //     client.getResponse().setResponse(static_cast<std::string>(&c));
            //     break;
            // }
            client.getResponse().getBodySize()--;
        }
    }
    std::cout << "Response: " << response << std::endl;
    std::cout << "Body size left: " << client.getResponse().getBodySize() << std::endl;
    if (!client.getResponse().getBodySize())
        client.setState(DONE);
}
