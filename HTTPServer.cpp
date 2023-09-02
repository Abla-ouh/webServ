/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:20:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/09/02 18:48:49 by abouhaga         ###   ########.fr       */
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
        //match_server_block(servers[i]);
        servers[i].CreateSocket(servers[i]);
        std:: cout << "Listening on port: " << RED << servers[i].getPort() << WHITE <<std::endl;
    }
}

void HTTPServer::removeClient(int clientSocket)
{
    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it).getClientSocket() == clientSocket)
        {
            close(clientSocket);
            clients.erase(it);
            std::cout << " Client Disconnected." << std::endl;
            break;
        }
    }
}

// void acceptNewClient(std::vector<server>& servers, std::vector<Client>& clients, fd_set& rd, fd_set& tmp_rd, int& maxSocket)
// {
//     std::vector<server>::iterator it = servers.begin();

//     while (it != servers.end())
//     {
//         if (FD_ISSET((*it).getServerSocket(), &tmp_rd))
//         {
//             Client newClient;
//             newClient.setClientSocket(accept((*it).getServerSocket(), NULL, NULL));
            
//             if (newClient.getClientSocket() == -1)
//             {
//                 std::cerr << "Failed to accept client connection" << std::endl;
//                 continue;
//             }
//             if (newClient.getClientSocket() > maxSocket)
//                 maxSocket = newClient.getClientSocket();
//             fcntl(newClient.getClientSocket(), F_SETFL, O_NONBLOCK);
//             newClient.setServer(*it);
//             FD_SET(newClient.getClientSocket(), &rd);
//             clients.push_back(newClient); // New client will be destoyed but its client_socket will keep the return from accept
//             std::cout << YELLOW <<"New Client request !" << WHITE << std::endl;
//         }
//         it++;
//     }
// }


void acceptNewClient(std::vector<server>& servers, std::vector<Client>& clients, fd_set& rd, fd_set& tmp_rd, int& maxSocket)
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
            
            const std::string hostHeader = newClient.getRequest().getHeader("Host");

            // Iterate through the server blocks to find a matching host
            for (std::vector<server>::iterator serverIt = servers.begin(); serverIt != servers.end(); ++serverIt)
            {
                if ((*serverIt).getServerName() == hostHeader)
                {
                    newClient.setServer(*serverIt);
                    newClient.hostMatched = true;
                    break;
                }
            }

            if (!newClient.hostMatched && !servers.empty())
            {
                newClient.setServer(servers.front());
            }
            
            FD_SET(newClient.getClientSocket(), &rd);
            clients.push_back(newClient); // New client will be destroyed, but its client_socket will keep the return from accept
            std::cout << YELLOW << "New Client request!" << WHITE << std::endl;
        }
        it++;
    }
}


void HTTPServer::start()
{
    fd_set readSet, writeSet, tmp_readSet, tmp_writeSet;
	int		err = 0;
    std::vector<Client>::iterator client_it;
    std::vector<server>::iterator server_it = this->servers.begin();
    int maxSocket = -1;

    // signal(SIGINT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    
    createConnections();
    while (server_it != servers.end())
    {
        FD_SET((*server_it).getServerSocket(), &readSet);
        if ((*server_it).getServerSocket() > maxSocket)
            maxSocket = (*server_it).getServerSocket();
        server_it++;
    }
    
    while(true)
    {
        tmp_readSet = readSet;
        tmp_writeSet = writeSet;

        //std::cout << "maxSocket: " << maxSocket << std::endl;
		err = select(maxSocket + 1, &tmp_readSet, &tmp_writeSet, NULL, NULL);
        if (err < 0)
        {
            std::perror("select() Error ");
            exit(1);
        }
        else
        {
            acceptNewClient(servers, clients, readSet, tmp_readSet, maxSocket); // each accepted client with its own virtual server
            client_it = clients.begin();
            while (this->clients.size() && client_it != clients.end())
            {
                if (FD_ISSET(client_it->getClientSocket(), &tmp_readSet))
                    handleRequest(*client_it, writeSet, readSet);

                if (FD_ISSET(client_it->getClientSocket(), &tmp_writeSet))
                {
                    response(*client_it);
                    if (client_it->getState() == DONE)
                    {
                        std::cout << "Dropping client " << std::endl;
                        if (client_it->getClientSocket() == maxSocket)
                            maxSocket--;
                        close(client_it->getResponse().getFileFd());
                        close(client_it->getClientSocket());
                        FD_CLR(client_it->getClientSocket(), &writeSet);
                        FD_CLR(client_it->getClientSocket(), &readSet);
                        client_it = clients.erase(client_it);
                        continue;
                    }
                }
                ++client_it;
            }
        }
    }
}
