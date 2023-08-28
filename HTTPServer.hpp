/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:55:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/28 14:02:08 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "./utils/configParse/serverClass.hpp"
#include "./utils/configParse/configFile.hpp"
//#include "Client.hpp"
//#include "Request.hpp"
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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <ios>

enum STATE
{
    BUILDING,
    SENDING,
    WAITING_CGI,
    DONE
};

enum RequestState {
    HEADER_READING,
    BODY_READING,
    PROCESSING,
    RESPONSE_SENDING,
    COMPLETED
};

class Request
{
    private:

    std::string method;
    std::string uri;
    std::string query;
    std::string version;

    std::map<std::string, std::string> headers;
    
    public:
        Request();
        void initRequest(const std::string& httpRequest);
        void parseHeaders(const std::string& headersBlock);
        bool isValid_URI_Char(char c);

        const std::string& getMethod() const;
        const std::string& getURI() const;
        const std::string& getQuery() const;
        const std::string& getVersion() const;
        const std::string& getHeader(const std::string& key) const;
    
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
class Client
{

    std::vector<location>   locations;
    Request                 request;
    Response                response;
    location                _location;
    int                     status;
    int                     client_socket;
    server                  _server;
    std::string             BodyFilename;
    STATE                   state;

    public:

        Client();
        ~Client();
        size_t                  hex_len;
        char                    hexBuff[20];
        char                    hexTempBuff[10];
        size_t                  chunk_size;
        bool                    waiting_for_chunk_size;
        bool                    ready;
        int                     firstTime;
        bool                    isBodyReady;
	    size_t                  _return;
        bool                    already_checked;
        int                     file;
	    bool                    readyToParse;
        std::string             file_name;
        std::string             header;
        size_t                  bodyPos;
        bool                    bodyChunked;
        RequestState            currentState;
        char                    data[8000];
        
        Request&                getRequest() { return request;};
        Response&               getResponse() { return response;};
        int                     getStatus() { return status;};
        location&               getlocation() { return _location; };
        std::vector<location>&  getlocations() { return locations; };
        int                     getClientSocket() { return client_socket;};
        server                  getServer() { return _server;};
        const std::string       &getBodyFilename() const { return BodyFilename;}
        STATE                   getState() { return state; };

        void            setRequest(Request other) { this->request = other; };
        void            setStatus(int other) { this->status = other; };
        void            setlocation(location other) { this->_location = other;};
        void            setlocations(std::vector<location> other) { this->locations = other; };
        void            setClientSocket(int other) { this->client_socket = other;};
        void            setServer(server other) { this->_server = other;};
        void            setCurrentState(RequestState state) {currentState = state;}
        void            setParsedRequestBodyFilename(const std::string &filename) { BodyFilename = filename;}
        void            setBodyReady(bool ready) { isBodyReady = ready;}
        void            setState(STATE other) { this->state = other; };
        RequestState    getCurrentState() const { return currentState;}
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
        void removeClient(int clientSocket);
        void handleRequest(Client &client, fd_set &writeSet, fd_set &readSet);
        std::string get_resource_type(const std::string& uri);
        //void handleDeleteRequest(int clientSocket, const std::string& uri, std::vector<server>& servers);

};

void        response(Client &client);
void        handleDeleteRequest(Client &client, std::string src);
void        locationMatching(std::string url, Client &client);

void        response(Client &client);
void        handleDeleteRequest(Client &client, std::string src);
void        locationMatching(std::string url, Client &client);
std::string get_resource_type(const char *res, Client client);
void		Post(Request req, location loc, Client &client);

size_t      getFileSize(const std::string& name);
char*       substr_no_null(const char* str, int start, int length, int str_len);
size_t      is_carriage(std::string str, Client &client);
#endif
