/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybel-hac <ybel-hac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:55:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/09/05 01:14:26 by ybel-hac         ###   ########.fr       */
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
#include <sys/wait.h>
#include <time.h>
#include <algorithm>
#include <ios>

enum STATE
{
    BUILDING,
    SENDING,
    FILE_READING,
    BUILDING_2,
    WAITING_CGI,
    SENDING_CGI,
	MOVING_FILE,
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
    std::map<std::string, std::string> cookies;
    
    public:
        Request();
        void initRequest(const std::string& httpRequest);
        void parseHeaders(const std::string& headersBlock);
        void parseCookies(const std::string& cookieHeader);
        bool isValid_URI_Char(char c);

        void setURI(const std::string& uri) { this->uri = uri; };
        std::string& getMethod();
        std::string& getURI();
        std::string& getQuery();
        std::string& getVersion();
        std::string& getHeader(const std::string& key);
        std::map<std::string, std::string>& getCookies() { return cookies;}
};

class Response
{
    std::string					body;
    std::string					location;
    std::string					redirection_url;
    std::string					response;
    std::string					old_url;
    int							file_fd;
    unsigned long long			body_size;
    std::map<int, std::string>	status_code;

public:
    Response();
	~Response(){};
    // ~Response() { if (file_fd) close(file_fd);};

    std::string 		&getBody();
    std::string 		getStatusLine(int status_code);
    std::string 		getDate();
    std::string 		getServer();
    std::string 		getContentType();
    std::string 		getContentLength();
    std::string 		&getResponse() { return response; };
    std::string 		getLocation() { return location; };
    std::string 		getLocationUrl() { return redirection_url; };
    int					getFileFd() { return file_fd; };
    unsigned long long&	getBodySize() { return body_size; };
    std::string			&getOldUrl() { return old_url; };

    void 				setLocation(std::string other) { location = other; };
    void 				setBody(std::string body) { this->body = body; };
    void 				setResponse(std::string response) { this->response = response; };
    void 				setLocationUrl(std::string other) { this->redirection_url = other; };
    void 				setFileFd(int other) { this->file_fd = other; };
    void 				setBodySize(size_t other) { this->body_size = other; };
    void 				setOldUrl(std::string other) { this->old_url = other; };
};
class Client
{

    std::vector<location>	locations;
    Request					request;
    Response				response;
    location				_location;
    int						status;
    int						client_socket;
    server					_server;
    STATE					state;
    int						cgiFd;
    pid_t					child_pid;
    time_t					start_time;
    std::string				BodyFilename;

public:
    Client();
    ~Client();
    int				err;
    size_t			hex_len;
    char			hexBuff[20];
    char			hexTempBuff[10];
    size_t			chunk_size;
    bool			waiting_for_chunk_size;
    bool			ready;
    int				firstTime;
    bool			isBodyReady;
    size_t			_return;
    bool			already_checked;
    int				file;
    bool			readyToParse;
    std::string		file_name;
    std::string		header;
    size_t			bodyPos;
    bool			bodyChunked;
    RequestState	currentState;
    char			data[8000];
    int				uploadedOutFile;
    int				uploadedInFile;
	long long		bodyReaded;
        Request&                getRequest() { return request;};
        Response&               getResponse() { return response;};
        int                     getStatus() { return status;};
        location&               getlocation() { return _location; };
        std::vector<location>&  getlocations() { return locations; };
        int                     getClientSocket() { return client_socket;};
        server                  getServer() { return _server;};
        const std::string       &getBodyFilename() const { return BodyFilename;}
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
        void            		setCurrentState(RequestState state) {currentState = state;}
        void            		setParsedRequestBodyFilename(const std::string &filename) { BodyFilename = filename;}
        void            		setBodyReady(bool ready) { isBodyReady = ready;}
        RequestState    		getCurrentState() const { return currentState;}
};


class HTTPServer {
    
    public:
        HTTPServer(configFile &obj);
        ~HTTPServer();
        void createConnections();
        void start();
        std::vector<server> servers;
        std::vector<Client> clients;

    // private:
    int handleRequest(std::vector<Client>::iterator &client_it, fd_set &writeSet, fd_set &readSet, int &maxSocket);
    void removeClient(std::vector<Client>::iterator &client_it, int &maxSocket);
    // void handleDeleteRequest(int clientSocket, const std::string& uri, std::vector<server>& servers);
};

void        response(Client &client);
void        handleDeleteRequest(Client &client, std::string src);
void        locationMatching(std::string url, Client &client);

void        response(Client &client);
void        handleDeleteRequest(Client &client, std::string src);
void        locationMatching(std::string url, Client &client);
std::string get_resource_type(const char *res, Client &client);
size_t      getFileSize(const std::string& name);
char*       substr_no_null(const char* str, int start, int length, int str_len);
size_t      is_carriage(std::string str, Client &client);
void		Post(Request& req, location& loc, Client &client);
string		generateName();
string		getIndexFromDirectory(Client& client, string directory);
void	    run_cgi(Client &client, string requestFile);
std::string	createAutoindexPage(string root_dir);
void		writeToNewFile(Client &client);
void		check_errors(Client &client, int code);
void		getFile(Client &client, int s);

void reselect(fd_set &readSet, fd_set &writeSet, int &maxSocket, std::vector<server> &servers, std::vector<Client> &clients);

#endif
