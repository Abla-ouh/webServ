/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelkhei <ebelkhei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:55:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/28 18:16:27 by ebelkhei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "./utils/configParse/serverClass.hpp"
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
#include <sys/wait.h> 
#include <unistd.h>
#include <time.h>
#include <algorithm>

enum STATE
{
    BUILDING,
    SENDING,
    FILE_READING,
    BUILDING_2,
    WAITING_CGI,
    SENDING_CGI,
    DONE
};

class Response
{
    std::string body;
    std::string location;
    std::string redirection_url;
    std::string response;
    std::string old_url;
    int         file_fd;
    unsigned long long      body_size;
    std::map<int, std::string>  status_code;

    public:
        Response() ;
        // ~Response() { if (file_fd) close(file_fd);};
        
        std::string &getBody();
        std::string getStatusLine(int status_code);
        std::string getDate();
        std::string getServer();
        std::string getContentType();
        std::string getContentLength();
        std::string &getResponse() { return response;};
        std::string getLocation() { return location; };
        std::string getLocationUrl() { return redirection_url; };
        int         getFileFd() { return file_fd; };
        unsigned long long      &getBodySize() { return body_size; };
        std::string &getOldUrl() { return old_url;};

        void        setLocation(std::string other) {location = other;};
        void        setBody(std::string body) { this->body = body;};
        void        setResponse(std::string response) { this->response = response; };
        void        setLocationUrl(std::string other) { this->redirection_url = other; };
        void        setFileFd(int other) { this->file_fd = other; };
        void        setBodySize(size_t other) { this->body_size = other; };
        void        setOldUrl(std::string other) { this->old_url = other; };

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
        std::string& getURI();
        const std::string& getQuery() const;
        const std::string& getVersion() const;
        const std::string& getHeader(const std::string& key) const;
		const std::string  getBody(){return ("this is temporary body");};
    
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
	int						cgiFd;
    pid_t                   child_pid;
    time_t                  start_time;

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
		int						getCgiFd() {return cgiFd;};
        pid_t                   getChildPid() { return child_pid; };
        time_t                  getStartTime() { return start_time; };

        void                    setChildPid(pid_t pid) { child_pid = pid; };
		void					setCgiFd(int fd) {cgiFd = fd;};
        void    				setRequest(Request other) { this->request = other; };
        void    				setStatus(int other) { this->status = other; };
        void    				setlocation(location other) { this->_location = other;};
        void    				setlocations(std::vector<location> other) { this->locations = other; };
        void    				setClientSocket(int other) { this->client_socket = other;};
        void    				setServer(server other) { this->_server = other;};
        void    				setState(STATE other) { this->state = other; };
        void                    setStartTime(time_t other) { this->start_time = other; };
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
std::string get_resource_type(const char *res, Client &client);
void		Post(Request& req, location& loc, Client &client);
string		generateName();
string		getIndexFromDirectory(Client& client, string directory);
std::string intToString(int number);
void	    run_cgi(Client &client, string requestFile);

#endif
