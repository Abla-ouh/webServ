#include "../HTTPServer.hpp"

location searchlocation(std::vector<location> locations, std::string _location)
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

void    checkResourceExistence(const char *res, int &fd, bool &isDir, Client &client)
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

void getFile(Client &client, int fd)
{
    char c;
    // check if location has CGI.
    // Else
    while ((read(fd, &c, 1)) > 0)
        client.getResponse().getBody() += c;
    client.setStatus(200);
    close(fd);
}

void getDir(Client &client, std::string src)
{
    std::vector<std::string>	indexes;
    std::string                 file;
    std::string                 tmp;
    int                         fd;

    tmp = client.getRequest().getURI();
    indexes = client.getlocation().getIndex();
    if (tmp[tmp.length() - 1] != '/')
    {
        client.getResponse().setLocation(client.getRequest().getURI() + "/");
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
                getFile(client, fd);
                return ;
            }
        }
    }
    if (client.getlocation().getAutoIndex() == "off")
        client.setStatus(403);
    // else return autoindex

}


void get(Client &client, std::string src)
{
    bool        isDir = false;
    int         fd = 0;

    checkResourceExistence(src.c_str(), fd, isDir, client);
    if (fd > 0)
    {
        std::cout << "isFile" << std::endl;
        getFile(client, fd);
    }
    else if (isDir)
    {
        std::cout << "isDir" << std::endl;
        getDir(client, src);
    }
}

void buildResponse(Client &client, std::string &response)
{
    std::stringstream ss;
    std::string crlf = "\r\n";

    ss << client.getResponse().getBody().size();
    response = client.getResponse().getStatusLine(client.getStatus()) + crlf;
    // response += "Date: " + client.getResponse().getDate() + crlf;
    response += "Server: " + client.getResponse().getServer() + crlf;
    if (client.getResponse().getLocationUrl().length())
        response += "location: " + client.getResponse().getLocationUrl() + crlf;
    response += "Content-Type: text/html" + crlf;
    response += "Content-Length: " + ss.str() + crlf;
    response += crlf;
    response += client.getResponse().getBody();
}

void check_redirections(Client &client)
{
    location &_location = client.getlocation();

    if (_location.getReturn().length())
    {
        if (_location.getRedirection() == "location")
        {
            client.setlocation(searchlocation(client.getlocations(), client.getlocation().getReturn()));
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

std::string get_resource_type(const char *res, Client &client)
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


int delete_directory_contents(const std::string& dir_path)
{
    DIR* dir = opendir(dir_path.c_str());
    dirent* entry;

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

void response(Client &client)
{
    std::string tmp;
    std::string src;
    std::string root;
    std::string response;

    locationMatching(client.getRequest().getURI(), client);
    root = client.getlocation().getRoot();
    tmp = client.getRequest().getURI();
        
    if (tmp[0] == '/' && root[root.length() - 1] == '/' && tmp.length() > 1)
        tmp.erase(0, 1);

    std::cout << tmp << std::endl;
    src = "." + root + tmp;
    
    std::cout << "Location: " << client.getlocation().getPath() << std::endl;
    std::cout << "|" << src << "|" << std::endl;
    if (!client.getStatus())
        check_redirections(client);

    if (!client.getStatus())
    {
        // std::cout << "Method: " << client.getRequest().getMethod() << std::endl;
        if (client.getRequest().getMethod() == "GET")
            get(client, src);
        // else if (client.getRequest().getMethod() == "POST")
        //     post(client);
        else if (client.getRequest().getMethod() == "DELETE")
            handleDeleteRequest(client, src);
    }
    buildResponse(client, response);
    std::cout << response << std::endl;
    close(client.getClientSocket());
    // std::cout << response << std::endl;
}