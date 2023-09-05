/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybel-hac <ybel-hac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:20:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/09/05 16:28:37 by ybel-hac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "HTTPServer.hpp"

HTTPServer::HTTPServer(configFile& obj) {
    std::vector<server> vec = obj.getServers();
    for (size_t i = 0; i < vec.size(); i++)
        servers.push_back(vec[i]);
}

HTTPServer::~HTTPServer()
{
    
}

// void match_server_block(Client &client)
// {
//     std::vector<server>::iterator it = servers.begin();
//     std::string hostHeader = client.getRequest().getHeader("Host");

//     while (it != servers.end())
//     {
//         if ((*it).getServerName() == hostHeader)
//         {
//             client.setServer(*it);
//             client.hostMatched = true;
//             break;
//         }
//         it++;
//     }
//     if (!client.hostMatched && !servers.empty())
//     {
//         client.setServer(servers.front());
//     }
// }

void HTTPServer::createConnections()
{
    //  initializing the socket for each server
    for (size_t i = 0; i < servers.size(); i++)
    {
        servers[i].CreateSocket(servers[i]);
        std:: cout << "Listening on port: " << RED << servers[i].getPort() << WHITE <<std::endl;
    }
}

void HTTPServer::removeClient(std::vector<Client>::iterator &client_it, int &maxSocket)
{
    std::cout << "Dropping client " << std::endl;

    if (client_it->getClientSocket() == maxSocket)
        maxSocket--;
    if (client_it->getStatus() != 500 && client_it->getResponse().getFileFd() != -1)
		close(client_it->getResponse().getFileFd());
    close(client_it->getClientSocket());
	if (client_it->getCgiFd() != -1)
		close(client_it->getCgiFd());
	if (client_it->file != -1)
		close(client_it->file);
	if (client_it->uploadedOutFile != -1)
		close(client_it->uploadedOutFile);
	if (client_it->uploadedInFile != -1)
		close(client_it->uploadedInFile);
    client_it = clients.erase(client_it);
}

void acceptNewClient(std::vector<server> &servers, std::vector<Client> &clients, fd_set &rd, fd_set &tmp_rd, int &maxSocket)
{
    //std::cout << "test" << std::endl;
    std::vector<server>::iterator it = servers.begin();

    while (it != servers.end())
    {
        if (FD_ISSET((*it).getServerSocket(), &tmp_rd))
        {
            Client newClient;
            newClient.setClientSocket(accept((*it).getServerSocket(), NULL, NULL));
            
            if (newClient.getClientSocket() == -1)
            {
                std::cerr << "Failed to accept client connection: " << strerror(errno) << std::endl;
                continue;
            }
            if (newClient.getClientSocket() > maxSocket)
                maxSocket = newClient.getClientSocket();
            fcntl(newClient.getClientSocket(), F_SETFL, O_NONBLOCK);
            newClient.setServer(*it);
            FD_SET(newClient.getClientSocket(), &rd);
            clients.push_back(newClient); // New client will be destoyed but its client_socket will keep the return from accept
            std::cout << YELLOW << "New Client request !" << WHITE << std::endl;
        }
        it++;
    }
}


void HTTPServer::start()
{
    fd_set readSet, writeSet, tmp_readSet, tmp_writeSet;
    std::vector<Client>::iterator client_it;
    std::vector<server>::iterator server_it = this->servers.begin();
    int maxSocket = -1;

	signal(SIGPIPE, SIG_IGN);
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    
    createConnections();
    while (server_it != servers.end())
    {
        server_it->setInternalErrPage(open(server_it->getErrorPage()["500"].c_str(), O_RDONLY));
        if (server_it->getInternalErrPage() == -1)
        {
            std::cerr << "Failed to open internal error page: " << strerror(errno) << std::endl;
            exit(1);
        }
        FD_SET((*server_it).getServerSocket(), &readSet);
        if ((*server_it).getServerSocket() > maxSocket)
            maxSocket = (*server_it).getServerSocket();
        server_it++;
    }
    
    while(true)
    {
        FD_ZERO(&tmp_readSet);
        FD_ZERO(&tmp_writeSet);

        tmp_readSet = readSet;
        tmp_writeSet = writeSet;

        if (select(maxSocket + 1, &tmp_readSet, &tmp_writeSet, NULL, NULL) < 0)
        {
            std::cout << "Select error: " << strerror(errno) << std::endl;
            reselect(readSet, writeSet, maxSocket, servers, clients);
            continue;
        }
        else
        {
            acceptNewClient(servers, clients, readSet, tmp_readSet, maxSocket);
            client_it = clients.begin();
            while (this->clients.size() && client_it != clients.end())
            {
                if (FD_ISSET((*client_it).getClientSocket(), &tmp_readSet))
				{
                    if (!handleRequest(client_it, writeSet, readSet, maxSocket))
						continue;
				}

                if (FD_ISSET((*client_it).getClientSocket(), &tmp_writeSet))
                {
                    response(*client_it);
                    if (client_it->getState() == DONE)
                    {
                        if (client_it->getChildPid() != 0)
                        {
                            kill(client_it->getChildPid(), SIGKILL);
                            client_it->setChildPid(0);
                        }
                        FD_CLR((*client_it).getClientSocket(), &writeSet);
						cout << PURPLE << "start CLIENT Dropped: " << client_it->getClientSocket() << WHITE "\n";
                        removeClient(client_it, maxSocket);
                        continue;
                    }
                }
                ++client_it;
            }
        }
    }
}
