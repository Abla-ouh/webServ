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
	_env["QUERY_STRING"] = req.getQuery();
	_env["REMOTE_ADDR"] = client.getServer().getHost();
	_env["REMOTE_IDENT"] = req.getHeader("Authorization");
	_env["REMOTE_USER"] = req.getHeader("Authorization");
	_env["REQUEST_METHOD"] = "POST";
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

void CGI::cgi_executor(Request& req, Client& client, string scritpPath, string requestFile)
{
	FILE*							infd = tmpfile();
	FILE*							outfd = tmpfile();
	char							**env;
	map<string, string>				obj = client.getlocation().getCgiPass();
	map<string, string>::iterator	interpreter = obj.find(scritpPath.substr(scritpPath.find_last_of(".") + 1));

	if (interpreter == obj.end())
		return (client.setState(DONE), client.setStatus(404));
	if (access(scritpPath.c_str(), R_OK))
		return (client.setState(DONE), client.setStatus(500), perror(scritpPath.c_str()));
	if (access(interpreter->second.c_str(), X_OK))
		return (client.setState(DONE), client.setStatus(500), perror(interpreter->second.c_str()));
	if (scritpPath.substr(scritpPath.length() - (interpreter->first.length() + 1)) != ("." + interpreter->first))
		return (client.setState(DONE), client.setStatus(404));
	client.setCgiFd(fileno(outfd));
	setCgiEnv(req, client, interpreter->second, scritpPath);
	env = getCgiEnv();
	client.setStartTime(time(0));
	client.setChildPid(fork());
	if (client.getChildPid() == -1)
		return (client.setStatus(500), perror("fork"));
	if (!client.getChildPid())
	{
		char	*tab[4] = {strdup(interpreter->second.c_str()), strdup(scritpPath.c_str()), strdup(requestFile.c_str()), 0};
		write(fileno(infd), req.getBody().c_str(), req.getBody().length());
		dup2(fileno(infd), STDIN_FILENO);
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

	cgi.cgi_executor(client.getRequest(), client, client.getlocation().getCgiPath(), requestFile);
}

// int main()
// {
// 	cgi_handler();
// 	return (0);
// }