#include "../configParse/configFile.hpp"
#include "../../HTTPServer.hpp"
#include <sys/types.h>
#include <dirent.h>

string	generateName()
{
	string final = "";
	srand((unsigned)time(0));
	for (int i = 0; i < 10; i++)
		final += 97 + rand() % 25;
	return (final);
}

void    Post(configFile conf, location loc)
{
	(void)conf;
	(void)loc;
	string body = "body";
	if (loc.getUploadPath()[0] == '/')
		loc.getUploadPath().erase(0, 1);
	string	uploadDir = loc.getRoot() + loc.getUploadPath();
	//? location support upload
	if (loc.getUploadPath().length() > 0)
	{
		if (access(uploadDir.c_str(), W_OK))
		{
			perror("access");
			exit(1);
		}
		ofstream file((uploadDir + "/" + generateName()).c_str());
		if (!file)
		{
			perror("file");
			exit(1);
		}
		file.write(body.c_str(), body.length());
	}
	
}