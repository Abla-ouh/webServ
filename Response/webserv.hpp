
#pragma once

#include <string>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sstream>
#include <algorithm> 
#include <map> 
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <list>
#include <utility>

class Location {
	private:
		std::string			_redirection;
		std::string			_equal;
		std::string			_path;
		std::vector<std::string>	_allow_methodes;
		std::string			_client_max_body_size;
		std::string			_root;
		std::vector<std::string>	_index;
		std::string			_autoindex;
		std::string			_upload_path;
		std::string			_return;
		// vector<cgi>		    _cgi_pass;
		std::string			_cgi_path;
		std::string			_cgi_ext;
	
    public:
		Location() : _equal("false"){};
		~Location(){};
		//?
		// ? seter's
		//
        void                setRedirection(std::string other) {this->_redirection = other; };
        // void                setReturn(std::string other) {this->_return = other; };
		void				setPath(std::string path){_path = path;};
		void				setAllowMethodes(std::string allow_methodes){_allow_methodes.push_back(allow_methodes);};
		void				setClient_max_body_size(std::string client_max){_client_max_body_size = client_max;};
		void				setRoot(std::string root){_root = root;};
		void				setIndex(std::string index){_index.push_back(index);};
		void				setUploadsPath(std::string upload_path){_upload_path = upload_path;};
		void				setAutoIndex(std::string autoindex){_autoindex = autoindex;};
		void				setEqual(std::string equal){_equal = equal;};
		void				setReturn(std::string ret){this->_return = ret;};
		// void				setCgiPass(cgi obj){_cgi_pass.push_back(obj);};
		void				setCgiPath(std::string cgi_path){_cgi_path = cgi_path;};
		void				setCgiExt(std::string cgi_ext){_cgi_ext = cgi_ext;};
		//?
		// ? geter's
		//?
        std::string             getRedirection() { return _redirection;};
        // std::string             getReturn() { return _return;};
		std::string				getEqual(){return (_equal);};
		std::string				getPath(){return (_path);};
		std::vector<std::string>&		getAllowMethodes(){return (_allow_methodes);};
		std::string				getClient_max_body_size(){return (_client_max_body_size);};
		std::string				getRoot(){return (_root);};
		std::vector<std::string>&		getIndex(){return (_index);};
		std::string&				getAutoIndex(){return (_autoindex);};
		std::string&				getUploadPath(){return (_upload_path);};		
		std::string&				getReturn(){return (_return);};
		// std::vector<cgi>			getCgiPass(){return (_cgi_pass);};
		std::string				getCgiPath(){return (_cgi_path);};
		std::string				getCgiExt(){return (_cgi_ext);};
};


class Request
{
    std::string method;
    std::string uri;
    std::string http_version;
    // HashMap     header;
    std::string body;

    public:
        std::string &getMethod() { return method; };
        std::string &getUri() { return uri; };
        std::string &getHttpVersion() { return http_version; };
        // HashMap     &getHeader() { return header; };
        std::string &getBody() { return body; };

        void        setMethod(std::string method) { this->method = method;};
        void        setUri(std::string uri) { this->uri = uri;};
        void        setHttpVersion(std::string http_version) { this->http_version = http_version;};
        // void        setHeader(HashMap header) { this->header = header;};
        void        setBody(std::string body) { this->body = body;};
};

class Response
{
    std::string body;
    std::string location;
    std::string redirection_url;
    std::map<int, std::string>  status_code;

    public:
        Response();

        std::string &getBody();
        std::string getStatusLine(int status_code);
        std::string getDate();
        std::string getServer();
        std::string getContentType();
        std::string getContentLength();
        std::string getLocation() { return location; };
        std::string getLocationUrl() { return redirection_url; };

        void        setLocation(std::string other) {location = other;};
        void        setBody(std::string body) { this->body = body;};
        void        setLocationUrl(std::string other) { this->redirection_url = other; };
};

class Client
{
    std::vector<Location>   locations;
    Request                 request;
    Response                response;
    Location                location;
    int                     status;

    public:
        Client()                { this->status = 0; };

        Request&                getRequest() { return request;};
        Response&               getResponse() { return response;};
        int                     getStatus() { return status;};
        Location&               getLocation() { return location; };
        std::vector<Location>&  getLocations() { return locations; };

        void    setRequest(Request other) { this->request = other; };
        void    setStatus(int other) { this->status = other; };
        void    setLocation(Location other) { this->location = other;};
        void    setLocations(std::vector<Location> other) { this->locations = other; };
};

void locationMatching(std::vector<Location> locations, std::string url, Client &client);