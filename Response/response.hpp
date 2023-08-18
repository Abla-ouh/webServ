#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "../HTTPServer.hpp"


// class Response
// {
//     std::string body;
//     std::string location;
//     std::string redirection_url;
//     std::map<int, std::string>  status_code;

//     public:
//         Response();

//         std::string &getBody();
//         std::string getStatusLine(int status_code);
//         std::string getDate();
//         std::string getServer();
//         std::string getContentType();
//         std::string getContentLength();
//         std::string getLocation() { return location; };
//         std::string getLocationUrl() { return redirection_url; };

//         void        setLocation(std::string other) {location = other;};
//         void        setBody(std::string body) { this->body = body;};
//         void        setLocationUrl(std::string other) { this->redirection_url = other; };
// };

// void        handleDeleteRequest(Client &client, std::string src);
// void        locationMatching(std::vector<location> locations, std::string url, Client &client);

#endif
