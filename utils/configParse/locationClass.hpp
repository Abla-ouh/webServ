#pragma once
#include "configFile.hpp"

using namespace std;

class location {
	private:
		string			_equal;
		string			_path;
		vector<string>	_allow_methodes;
		string			_client_max_body_size;
		string			_root;
		vector<string>	_index;
		string			_autoindex;
		string			_upload_path;
	public:
		location() : _equal("false"){};
		~location(){};
		//?
		// ? seter's
		//?
		void				setPath(string path){_path = path;};
		void				setAllowMethodes(string allow_methodes){_allow_methodes.push_back(allow_methodes);};
		void				setClient_max_body_size(string client_max){_client_max_body_size = client_max;};
		void				setRoot(string root){_root = root;};
		void				setIndex(string index){_index.push_back(index);};
		void				setUploadsPath(string upload_path){_upload_path = upload_path;};
		void				setAutoIndex(string autoindex){_autoindex = autoindex;};
		void				setEqual(string equal){_equal = equal;};
		//?
		// ? geter's
		//?
		string&				getEqual(){return (_equal);};
		string&				getPath(){return (_path);};
		vector<string>&		getAllowMethodes(){return (_allow_methodes);};
		string&				getClient_max_body_size(){return (_client_max_body_size);};
		string&				getRoot(){return (_root);};
		vector<string>&		getIndex(){return (_index);};
		string&				getAutoIndex(){return (_autoindex);};
		string&				getUploadPath(){return (_upload_path);};		
};
