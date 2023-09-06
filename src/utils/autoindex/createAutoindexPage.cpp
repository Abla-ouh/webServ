#include "../configParse/configFile.hpp"

// * the function create a autoindex page dependibg on the directory passed as argument
// @param root_dir the directory want to create autoindex of it
// @return the function return 0 in error and 1 in success
std::string	createAutoindexPage(string root_dir)
{
	struct dirent *de;
	std::string page;
	DIR *dir = opendir(root_dir.c_str());

	page += "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
\t<head>\n\
	\t<meta charset=\"UTF-8\">\n\
	\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
	\t<title>الفهرس</title>\n\
	\t<style>\n\
\t\t\t@import url('https://fonts.googleapis.com/css2?family=Roboto&display=swap');\n\
\t\t\t* {\n\
\t\t\t\tmargin: 0;\n\
\t\t\t\tpadding: 0;\n\
\t\t\t\ttext-decoration: none;\n\
\t\t\t}\n\
\t\t\n\
\t\t\tbody {\n\
\t\t\t\tfont-family: 'Roboto', sans-serif;\n\
\t\t\t\tbackground-color: #2b2d42;\n\
\t\t\t}\n\
\t\t\n\
\t\t\t.container {\n\
\t\t\t\tdisplay: flex;\n\
\t\t\t\tflex-direction: column;\n\
\t\t\t\tjustify-content: center;\n\
\t\t\t\talign-items: center;\n\
\t\t\t\tborder: #edf2f4 2px solid;\n\
\t\t\t\tborder-radius: 20px;\n\
\t\t\t\twidth: fit-content;\n\
\t\t\t\tposition: absolute;\n\
\t\t\t\ttop: 2%;\n\
\t\t\t\tleft: 50%;\n\
\t\t\t\ttransform: translate(-50%, 0%);\n\
\t\t\t\tpadding: 30px 50px;\n\
\t\t\t}\n\
\t\t\n\
\t\t\t.container a {\n\
\t\t\t\tmargin: 20px 0;\n\
\t\t\t\tpadding: 5px;\n\
\t\t\t\tcolor: #6096ba;\n\
\t\t\t\tfont-size: 20px;\n\
\t\t\t\ttransition: color ease-in-out .2s;\n\
\t\t\t}\n\
\t\t\n\
\t\t\t.container a:hover {\n\
\t\t\t\tcolor: #274c77;\n\
\t\t\t}\n\
\t\t\t.container h1 {\n\
\t\t\t\tcolor: #edf2f4;\n\
\t\t\t\tmargin: 20px 0;\n\
\t\t\t}\n\
\t\t</style>\n\
\t</head>\n\
\t<body>\n\
\t\t<div class=\"container\">\n\
\t\t\t<h1>الفهرس</h1>\n";
	if (dir) {
		while ((de = readdir(dir)))
		{
			string link = "";
			link = ("\t\t\t<a href=\"" + string(de->d_name) + "\"" + ">" + string(de->d_name) + "</a>\n");
			page += link + "\n";
		}
		closedir(dir);
	}
	page += "\t\t</div>\n";
	page += "\t</body>\n";
	page += "</html>\n";
	return (page);
}
