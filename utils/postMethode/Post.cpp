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

void Post(Request req, location loc)
{
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
	else
	{
		if (get_resource_type(req.getURI()) == "FILE") // * is file
		{
			if (loc.isCgi())
				; // contain cgi
			else
				sendErrorResponse(clientSocket, "403 Forbidden"); // * is dir
		}
		else if (get_resource_type(req.getURI()) == "DIRE")
		{
			if (req.getURI()[req.getURI().length()] != '/')
				;
		}
		else
			throw (unknownError());
	}
}
