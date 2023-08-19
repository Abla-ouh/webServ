#include "webserv.hpp"

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