/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelkhei <ebelkhei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:55:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/22 15:28:53 by ebelkhei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "./utils/configParse/serverClass.hpp"
//#include "Client.hpp"
//#include "Request.hpp"
#include "./utils/configParse/configFile.hpp"
//#include "./Response/response.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <errno.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>

enum STATE
{
    BUILDING,
    SENDING,
    WAITING_CGI,
    DONE
};

class Response
{
    std::string body;
    std::string location;
    std::string redirection_url;
    std::string response;
    std::map<int, std::string>  status_code;

    public:
        Response();

        std::string &getBody();
        std::string getStatusLine(int status_code);
        std::string getDate();
        std::string getServer();
        std::string getContentType();
        std::string getContentLength();
        std::string &getResponse() { return response;};
        std::string getLocation() { return location; };
        std::string getLocationUrl() { return redirection_url; };

        void        setLocation(std::string other) {location = other;};
        void        setBody(std::string body) { this->body = body;};
        void        setResponse(std::string response) { this->response = response; };
        void        setLocationUrl(std::string other) { this->redirection_url = other; };
};

class Request
{
    private:

    std::string method;
    std::string uri;
    std::string query;
    std::string version;

    std::map<std::string, std::string> headers;
    void parseHeaders(const std::string& headersBlock);
    
    public:
        Request();
        void initRequest(const std::string& httpRequest);

        const std::string& getMethod() const;
        const std::string& getURI() const;
        const std::string& getQuery() const;
        const std::string& getVersion() const;
        const std::string& getHeader(const std::string& key) const;
    
};

class Client
{
    std::vector<location>   locations;
    Request                 request;
    Response                response;
    location                _location;
    int                     status;
    int                     client_socket;
    server                  _server;
    STATE                   state;


    public:

        Client();
        ~Client();

        char                    data[8000];
        Request&                getRequest() { return request;};
        Response&               getResponse() { return response;};
        int                     getStatus() { return status;};
        location&               getlocation() { return _location; };
        std::vector<location>&  getlocations() { return locations; };
        int                     getClientSocket() { return client_socket;};
        server                  getServer() { return _server;};
        STATE                   getState() { return state; };


        void    setRequest(Request other) { this->request = other; };
        void    setStatus(int other) { this->status = other; };
        void    setlocation(location other) { this->_location = other;};
        void    setlocations(std::vector<location> other) { this->locations = other; };
        void    setClientSocket(int other) { this->client_socket = other;};
        void    setServer(server other) { this->_server = other;};
        void    setState(STATE other) { this->state = other; };
};


class HTTPServer {
    
    public:
        HTTPServer(configFile &obj);
        ~HTTPServer();
        void createConnections();
        void start();
        std::vector<server> servers;
        std::vector<Client> clients;

    //private:
    
        void readFromFile(std::string file, std::string &str);
        void addClient(int clientSocket);
        void removeClient(int clientSocket);
        void handleRequest(Client &client, fd_set &writeSet, fd_set &readSet);
        void sendResponse(int clientSocket);
        void sendErrorResponse(int clientSocket, const std::string& statusLine);
        std::string get_resource_type(const std::string& uri);
        //void handleDeleteRequest(int clientSocket, const std::string& uri, std::vector<server>& servers);

};

void        response(Client &client);
void        handleDeleteRequest(Client &client, std::string src);
void        locationMatching(std::string url, Client &client);
std::string get_resource_type(const char *res, Client client);
void		Post(Request req, location loc, Client &client);
#endif
