#include "../HTTPServer.hpp"

Response::Response()
{
    
    this->status_code[200] = "OK";
    this->status_code[201] = "Created";
    this->status_code[501] = "Not Implemented";
    this->status_code[400] = "Bad Request";
    this->status_code[403] = "Forbidden";
    this->status_code[404] = "Not Found";
    this->status_code[405] = "Method Not Allowed";
    this->status_code[413] = "Request Entity Too Large";
    this->status_code[414] = "Request-URI Too Long";
    this->status_code[301] = "Moved Permanently";
    this->status_code[500] = "Internal Server Error";
    this->status_code[505] = "HTTP Version Not Supported";
    this->status_code[204] = "No Content";
    this->status_code[504] = "Gateway Timeout";

    this->body_size = 0;
    this->file_fd = 0;
}

std::string Response::getStatusLine(int code)
{
    std::string status_line;
    std::stringstream ss;

    ss << code;
    status_line = "HTTP/1.1 " + ss.str() + ' ' + status_code[code];
    return (status_line);
}

std::string Response::getDate()
{
    char buffer[32];

    // std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    // std::tm* timeinfo = std::gmtime(&currentTime);
    // std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return (buffer);
}

std::string Response::getServer()
{
    return ("webserv/1.0");
}

std::string Response::getContentType()
{
    return "idk :)";
}

std::string Response::getContentLength()
{
    std::stringstream ss;

    ss << body.size();
    return (ss.str());
}

std::string &Response::getBody()
{
    return body;
}

void reselect(fd_set &readSet, fd_set &writeSet, int &maxSocket, std::vector<server> &servers, std::vector<Client> &clients)
{
    std::vector<Client>::iterator client_it = clients.begin();
    std::vector<server>::iterator server_it = servers.begin();

    for (; clients.size() && client_it < clients.end(); client_it++)
    {
        if (client_it->getChildPid() != 0)
        {
            kill(client_it->getChildPid(), SIGKILL);
            client_it->setChildPid(0);
        }
        close(client_it->getClientSocket());
        close(client_it->getCgiFd());
        close(client_it->getResponse().getFileFd());
    }

    clients.clear();

    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);

    maxSocket = -1;

    while (server_it != servers.end())
    {
        FD_SET((*server_it).getServerSocket(), &readSet);
        if ((*server_it).getServerSocket() > maxSocket)
            maxSocket = (*server_it).getServerSocket();
        server_it++;
    }
}
