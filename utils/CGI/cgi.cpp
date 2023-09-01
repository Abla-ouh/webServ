#include "CGI.hpp"

using namespace std;


void	CGI::setCgiEnv(Request req, Client &client, string interpreter, string scritpPath)
{
	_client = client;
	_env["HTTP_HOST"] = req.getHeader("Host"); // ! check
	_env["PATH"] = client.getServer().getRoot();
	_env["CONTENT_LENGTH"] = req.getHeader("Content-Length");
	_env["CONTENT_TYPE"] = req.getHeader("Content-Type");
	_env["REQUEST_URI"] = req.getURI();
	_env["AUTH_TYPE"] = "1.1";
	_env["PAHT_INFO"] = client.getlocation().getPath();
	_env["PAHT_TRANSLATED"] = client.getlocation().getPath();
	_env["QUERY_STRING"] = !req.getQuery().empty() ? req.getQuery().substr(1) : "";
	_env["REMOTE_ADDR"] = client.getServer().getHost();
	_env["REMOTE_IDENT"] = req.getHeader("Authorization");
	_env["REMOTE_USER"] = req.getHeader("Authorization");
	_env["REQUEST_METHOD"] = req.getMethod();
	_env["REQUEST_URI"] = req.getURI();
	_env["SCRIPT_NAME"] = interpreter;
	_env["SCRIPT_FILENAME"] = scritpPath;
	// cout << RED + itt->lang << "\n";
	_env["SERVER_NAME"] = _env["REMOTE_ADDR"];
	_env["SERVER_PORT"] = client.getServer().getPort();
	_env["SERVER_PROTOCOL"] = ("HTTP/" + req.getVersion());
	_env["SERVER_NAME"] = client.getServer().getHost();
	_env["SERVER_SOFTWARE"] = client.getServer().getServerName() + '/' + req.getVersion();
	_env["GATEWAY_INTERFACE"] = "1.1";
	_env["REDIRECT_STATUS"] = "200";
}

char	**CGI::getCgiEnv()
{
	size_t	len = 0;
	char	**old_env = _client.getServer().getEnv();
	size_t	i = 0;

	while (old_env[len])
		len++;
	char	**env = new char*[len + _env.size() + 2];

	for (; i < len; i++)
		env[i] = strdup(old_env[i]);
	for (map<string, string>::iterator itt = _env.begin(); itt != _env.end(); itt++)
		env[i++] = strdup((itt->first + "=" + itt->second).c_str());
	env[i] = 0;
	return (env);
}

void CGI::cgi_executor(Client& client, string scritpPath, string requestFile, string interpreter)
{
	FILE*							outfd = tmpfile();
	char							**env;

	if (interpreter[0] != '/')
		interpreter = client.getlocation().getRoot() + '/' + interpreter;
	client.setCgiFd(fileno(outfd));
	setCgiEnv(client.getRequest(), client, interpreter, scritpPath);
	env = getCgiEnv();
	client.setStartTime(time(0));
	client.setChildPid(fork());
	if (client.getChildPid() == -1)
		return (client.setStatus(500), perror("fork"));
	if (!client.getChildPid())
	{
		char	*tab[4] = {strdup(interpreter.c_str()), strdup(scritpPath.c_str()), strdup(requestFile.c_str()), 0};
		dup2(client.file, STDIN_FILENO);
		dup2(fileno(outfd), STDOUT_FILENO);
		dup2(fileno(outfd), STDERR_FILENO);
		execve(tab[0], tab, env);
		client.setStatus(500);
		perror("execve");
		exit(1);
	}

	// if (waitpid(-1, &status, WNOHANG) == -1)
	// 	client.setStatus(500);
	// else if (waitpid(-1, &status, WNOHANG))
	// {
	// 	client.setCgiFd(fileno(outfd));
	// 	client.setState(DONE);
	// }
	// else
	// 	client.setState(WAITING_CGI);
}

void	run_cgi(Client &client, string requestFile)
{
	cout << "**************** run_cgi ****************\n";
	CGI		cgi;
	string	scriptPath;
	map<string, string>				obj = client.getlocation().getCgiPass();
	map<string, string>::iterator	interpreter = obj.find(requestFile.substr(requestFile.find_last_of(".") + 1));

	if (client.getlocation().getCgiPath()[0] != '/')
		client.getlocation().setCgiPath(client.getlocation().getRoot() + "/" + client.getlocation().getCgiPath());
	if (access(requestFile.c_str(), R_OK)) // ? check file is exist and have write permession
		return (client.setStatus(500), perror(requestFile.c_str()));
	if (interpreter == obj.end())
	{
		if (!client.getlocation().getCgiPath().empty())
		{
			interpreter = obj.find(client.getlocation().getCgiPath().substr(client.getlocation().getCgiPath().find_last_of(".") + 1));
			if (interpreter == obj.end())
				return (client.setStatus(404));
			if (access(interpreter->second.c_str(), X_OK))
				return (client.setStatus(500), perror(interpreter->second.c_str()));
			cgi.cgi_executor(client, client.getlocation().getCgiPath(), requestFile, interpreter->second);
			return ;
		}
	}
	if (interpreter != obj.end() && access(interpreter->second.c_str(), X_OK))
		return (client.setStatus(500), perror(interpreter->second.c_str()));
	// if (requestFile.substr(requestFile.length() - (interpreter->first.length() + 1)) != ("." + interpreter->first))
	// 	return (client.setState(DONE), client.setStatus(404));

	cgi.cgi_executor(client, client.getlocation().getRoot() + '/' + client.getRequest().getURI(), requestFile, interpreter->second);
}

// int main()
// {
// 	cgi_handler();
// 	return (0);
// }