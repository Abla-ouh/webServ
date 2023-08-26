#include "../HTTPServer.hpp"

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
    int fd = client.getResponse().getFileFd();
    // check if location has CGI.
    // Else

    while ((a = read(fd, &c, 1)) > 0 && max--)
        client.getResponse().getBodySize()++;

    std::cout << "a: " << a << std::endl;

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
        // std::cout << "indexes" << std::endl;
        for (size_t i = 0; i < indexes.size(); i++)
        {
            file = src + indexes[i];
            std::cout << "FILE: " << file << std::endl;
            if ((fd = open(file.c_str(), O_RDONLY)) > 0)
            {
                client.getResponse().setFileFd(fd);
                client.setState(FILE_READING);
                return;
            }
        }
        client.setStatus(404);
    }
    std::cout << "autoindex: " << client.getlocation().getAutoIndex() << std::endl;
    if (client.getlocation().getAutoIndex() == "off")
        client.setStatus(403);
    else
    {
        std::cout << "autoindex" << std::endl;
        client.getResponse().setBody(createAutoindexPage(src));
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
    // response += "Date: " + client.getResponse().getDate() + crlf;
    response += "Server: " + client.getResponse().getServer() + crlf;
    if (client.getResponse().getLocationUrl().length())
        response += "Location: " + client.getResponse().getLocationUrl() + crlf;
    // response += "Content-Type: text/html" + crlf;
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

std::string get_resource_type(const char *res, Client client)
{
    DIR *dir;

    if (!access(res, F_OK))
    {
        if (!access(res, W_OK))
        {
            if (!(dir = opendir(res)))
                return "FILE";
            else
            {
                closedir(dir);
                return "DIRE";
            }
        }
        else
            client.setStatus(403);
    }
    else
        client.setStatus(404);
    return "INVALID";
}

int delete_directory_contents(const std::string &dir_path)
{
    DIR *dir = opendir(dir_path.c_str());
    dirent *entry;

    if (!dir)
    {
        std::cerr << "Error opening directory " << dir_path << std::endl;
        return 1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        std::string entry_path = dir_path + "/" + entry->d_name;
        if (entry->d_type == DT_DIR)
        {
            delete_directory_contents(entry_path);
            rmdir(entry_path.c_str());
        }
        else
            unlink(entry_path.c_str());
    }
    closedir(dir);
    return 0;
}

void handleDeleteRequest(Client &client, std::string src)
{
    std::string full_path = src;
    std::string rcs_type = get_resource_type(full_path.c_str(), client);

    if (client.getStatus())
        return;
    if (rcs_type == "FILE")
    {
        // Handle file deletion
        if (unlink(full_path.c_str()) == 0)
            // Successfully deleted the file
            client.setStatus(204);
        // sendResponse(client.getClientSocket());
        else
            // Error while deleting the file
            client.setStatus(500);
        // sendErrorResponse(client.getClientSocket(), "500 Internal Server Error");
    }
    else if (rcs_type == "DIRE")
    {
        // Handle directory deletion
        if (delete_directory_contents(full_path) == 0 && rmdir(full_path.c_str()) == 0)
            // Successfully deleted the directory
            client.setStatus(204);
        else
            // Error while deleting the directory
            client.setStatus(500);
    }
    // else
    //     // Handle invalid resource type
    //     client.setStatus(400);
}

void sendResponse(Client &client);

void check_errors(Client &client, int code)
{
    std::stringstream ss;
    int fd;

    ss << code;
    std::map<std::string, std::string> errors = client.getServer().getErrorPage();
    fd = open(errors[ss.str()].c_str(), O_RDONLY);
    client.getResponse().setFileFd(fd);
    getFile(client, 0);
}

void response(Client &client)
{
    std::string tmp;
    std::string src;
    std::string root;
    std::string &response = client.getResponse().getResponse();

    // std::cout << "STATE: " << client.getState() << std::endl;

    if (client.getState() == BUILDING)
    {
        client.getResponse().setOldUrl(client.getRequest().getURI());
        locationMatching(client.getRequest().getURI(), client);
        root = client.getlocation().getRoot();
        tmp = client.getRequest().getURI();

        if (tmp[0] == '/' && root[root.length() - 1] == '/' && tmp.length() > 1)
            tmp.erase(0, 1);

        src = root + tmp;
        // std::cout << "SRC: " << src << std::endl;

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
            client.setState(FILE_READING);
            check_errors(client, client.getStatus());
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
    }

    else if (client.getState() == SENDING)
        sendResponse(client);
        // client.setState(DONE);
}

// Check if the client close the connection before sending

void sendResponse(Client &client)
{
    size_t size = 2048;
    int     i = 0;
    int     a = 0;
    std::string response;
    char c;

    // std::cout << "Sending response" << std::endl;
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
    // std::cout << "Response: " << response << std::endl;
    std::cout << "Body size left: " << client.getResponse().getBodySize() << std::endl;
    if (!client.getResponse().getBodySize())
        client.setState(DONE);
}
