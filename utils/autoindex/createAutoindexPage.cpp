#include "../configParse/configFile.hpp"

// * the function create a autoindex page dependibg on the directory passed as argument
// @param root_dir the directory want to create autoindex of it
// @return the function return 0 in error and 1 in success
std::string	createAutoindexPage(string root_dir)
{
	struct dirent *de;
	std::string page;
	// ofstream page((root_dir + '/' + "autoindex.html").c_str());
	DIR *dir = opendir(root_dir.c_str());

	if (!dir)
		return ("");
	page += "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
\t<head>\n\
	\t<meta charset=\"UTF-8\">\n\
	\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
	\t<title>Autoindex</title>\n\
	\t<style>\n\
	\t\t@import url('https://fonts.googleapis.com/css2?family=Roboto&display=swap');\n\
\t\t\t* {\n\
	\t\t\tmargin: 0;\n\
	\t\t\tpadding: 0;\n\
	\t\t\ttext-decoration: none;\n\
\t\t\t}\n\
\n\
\t\t\tbody {\n\
	\t\t\tfont-family: 'Roboto', sans-serif;\n\
	\t\t\tbackground-color: #000000;\n\
\t\t\t}\n\
\t\t\t.container {\n\
	\t\t\tdisplay: flex;\n\
	\t\t\tflex-direction: column;\n\
	\t\t\tjustify-content: center;\n\
	\t\t\talign-items: center;\n\
\t\t\t}\n\
\n\
\t\t\t.container a {\n\
	\t\t\tmargin: 20px 0;\n\
	\t\t\tpadding: 5px;\n\
	\t\t\tcolor: #f72585;\n\
	\t\t\tfont-size: 20px;\n\
	\t\t\ttransition: color ease-in-out .2s;\n\
\t\t\t}\n\
\n\
\t\t\t.container a:hover {\n\
	\t\t\tcolor: rgb(241, 108, 168);\n\
\t\t\t}\n\
	\t</style>\n\
\t</head>\n\
\t<body>\n\
\t\t<div class=\"container\">\n";
	while ((de = readdir(dir)))
	{
		string link = "";
		link = ("\t\t\t<a href=\"" + string(de->d_name) + "\"" + ">" + string(de->d_name) + "</a>\n");
		page += link + "\n";
	}
	page += "\t\t</div>\n";
	page += "\t</body>\n";
	page += "</html>\n";
	closedir(dir);
	return (page);
}
