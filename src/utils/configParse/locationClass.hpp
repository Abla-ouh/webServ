#pragma once
#include "configFile.hpp"

using namespace std;

typedef struct s_cgi
{
	string	lang;
	string	path;
} cgi;


class location {
	private:
		string				_equal;
		string				_redirection;
		string				_path;
		vector<string>		_allow_methodes;
		string				_root;
		vector<string>		_index;
		string				_autoindex;
		string				_upload_path;
		string				_return;
		bool				_hasCgi;
		map<string, string>	_cgi_pass;
		string				_cgi_path;
	public:
		location() : _equal("false"), _redirection(""), _root(""), _hasCgi(0){};
		~location(){};
		//?
		// ? seter's
		//?
		void				setPath(string path){_path = path;};
		void				setAllowMethodes(string allow_methodes){_allow_methodes.push_back(allow_methodes);};
		void				setRoot(string root){_root = root;};
		void				setIndex(string index){_index.push_back(index);};
		void				setUploadsPath(string upload_path){_upload_path = upload_path;};
		void				setAutoIndex(string autoindex){_autoindex = autoindex;};
		void				setEqual(string equal){_equal = equal;};
		void				setReturn(string ret){this->_return = ret;};
		void				setCgiPass(string ext, string path){_cgi_pass[ext] = path;};
		void				setRedirection(string red){_redirection = red;};
		void				setHasCgi(bool cgi){_hasCgi = cgi;};
		void				setCgiPath(string path){_cgi_path = path;};
		//?
		// ? geter's
		//?
		string				getEqual(){return (_equal);};
		string				getPath(){return (_path);};
		vector<string>&		getAllowMethodes(){return (_allow_methodes);};
		string				getRoot(){return (_root);};
		vector<string>&		getIndex(){return (_index);};
		string&				getAutoIndex(){return (_autoindex);};
		string&				getUploadPath(){return (_upload_path);};		
		string&				getReturn(){return (_return);};
		map<string, string>	getCgiPass(){return (_cgi_pass);};
		string				getCgiPath(){return (_cgi_path);};
		string&				getRedirection(){return (_redirection);};
		bool				isCgi(){return (_hasCgi);};
};
