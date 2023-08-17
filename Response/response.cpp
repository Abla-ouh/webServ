#include "webserv.hpp"

Location searchLocation(std::vector<Location> locations, std::string location)
{
    Location def;

    for (size_t i = 0; i < locations.size(); i++)
    {
        if (locations[i].getPath() == location)
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
    int                         fd;

    indexes = client.getLocation().getIndex();
    if (client.getRequest().getUri().back() != '/')
    {
        client.getResponse().setLocation(client.getRequest().getUri() + "/");
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
                getFile(client, fd);
                return ;
            }
        }
    }
    if (client.getLocation().getAutoIndex() == "off")
        client.setStatus(403);
    // else return autoindex

}


void get(Client &client)
{
    std::string src;
    std::string tmp;
    bool        isDir = false;
    int         fd = 0;

    tmp = client.getRequest().getUri();
    if (tmp.front() == '/' && client.getLocation().getRoot().back() == '/' && tmp.length() > 1)
        tmp.erase(0, 1);

    src = "." + client.getLocation().getRoot() + tmp;
    std::cout << "Src: " << src << std::endl;
    checkResourceExistence(src.c_str(), fd, isDir, client);

    // std::cout << "Fd: " << fd << std::endl;
    // std::cout << "Dir: " << isDir << std::endl;
    if (fd > 0)
        getFile(client, fd);
    else if (isDir)
        getDir(client, src);
}

void buildResponse(Client &client, std::string &response)
{
    std::string crlf = "\r\n";

    response = client.getResponse().getStatusLine(client.getStatus()) + crlf;
    // response += "Date: " + client.getResponse().getDate() + crlf;
    response += "Server: " + client.getResponse().getServer() + crlf;
    if (client.getResponse().getLocationUrl().length())
        response += "Location: " + client.getResponse().getLocationUrl() + crlf;
    response += "Content-Type: " + client.getResponse().getContentType() + crlf;
    response += "Content-Length: " + client.getResponse().getContentLength() + crlf;
    response += crlf;
    response += client.getResponse().getBody();
}

void check_redirections(Client &client)
{
    Location &location = client.getLocation();

    if (location.getReturn().length())
    {
        if (location.getRedirection() == "location")
        {
            client.setLocation(searchLocation(client.getLocations(), client.getLocation().getReturn()));
            if (!client.getLocation().getPath().length())
                client.setStatus(404);
        }
        else
        {
            client.getResponse().setLocationUrl(location.getReturn());
            client.setStatus(301);
        }
    }
}

void response(Client &client)
{
    std::string response;

    locationMatching(client.getLocations(), client.getRequest().getUri(), client);

    if (!client.getStatus())
        check_redirections(client);

    if (!client.getStatus())
    {
        if (client.getRequest().getMethod() == "GET")
            get(client);
        // else if (client.getRequest().getMethod() == "POST")
        //     post(client);
        // else if (client.getRequest().getMethod() == "DELETE")
        //     delete(client);
    }
    buildResponse(client, response);
    std::cout << response << std::endl;
}


// int main()
// {
//     Client client;
//     Location location;
//     std::vector<Location> locs;
//     Location loc1;

//     loc1.setPath("/test");
//     // loc2.setPath("/test/lol/");
//     // loc3.setPath("/testt/lol2");

//     // loc1.setReturn("/test/lol/");
//     // loc1.setRedirection("location");

//     loc1.setRoot("/");
//     // loc2.setRoot("/test/");
//     // loc3.setRoot("/test/lol2");

//     loc1.getIndex().push_back("index.html");
//     loc1.getIndex().push_back("indexd.html");
//     // loc1.setAutoIndex("off");
//     // loc2.setAutoIndex("off");

    
//     locs.push_back(loc1);
//     // locs.push_back(loc2);
//     // locs.push_back(loc3);


//     location.getAllowMethodes().push_back("GET");
//     client.getRequest().setMethod("GET");
    
//     client.getRequest().setUri("/test/img.jpg");
//     client.setLocations(locs);


//     response(client);
// }
