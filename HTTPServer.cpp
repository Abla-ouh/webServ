/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:20:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/28 14:03:39 by abouhaga         ###   ########.fr       */
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

void HTTPServer::createConnections()
{
    //  initializing the socket for each server
    for (size_t i = 0; i < servers.size(); i++)
    {
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

void acceptNewClient(std::vector<server>& servers, std::vector<Client>& clients, fd_set& rd, fd_set& tmp_rd, int& maxSocket)
{
    std::vector<server>::iterator it = servers.begin();

    while (it != servers.end())
    {
        if (FD_ISSET((*it).getServerSocket(), &tmp_rd))
        {
            Client newClient;
            newClient.setClientSocket(accept((*it).getServerSocket(), NULL, NULL));
            
            if (newClient.getClientSocket() == -1)
            {
                std::cerr << "Failed to accept client connection" << std::endl;
                continue;
            }
            if (newClient.getClientSocket() > maxSocket)
                maxSocket = newClient.getClientSocket();
            fcntl(newClient.getClientSocket(), F_SETFL, O_NONBLOCK);
            newClient.setServer(*it);
            FD_SET(newClient.getClientSocket(), &rd);
            clients.push_back(newClient); // New client will be destoyed but its client_socket will keep the return from accept
            std::cout << "New Client request !" << std::endl;
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

    signal(SIGINT, SIG_IGN);

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
        
        if (select(maxSocket + 1, &tmp_readSet, &tmp_writeSet, NULL, NULL) == -1)
            std::cerr << "Error in Select !" << std::endl;
        
        else
        {
            acceptNewClient(servers, clients, readSet, tmp_readSet, maxSocket); // each accepted client with its own virtual server
            client_it = clients.begin();
            while (this->clients.size() && client_it != clients.end())
            {
                // std::cout << "Clients size: " << clients.size() << std::endl;
                if (FD_ISSET((*client_it).getClientSocket(), &tmp_readSet))
                    handleRequest(*client_it, writeSet, readSet);

                if (FD_ISSET((*client_it).getClientSocket(), &tmp_writeSet))
                {
                    std::cout << "here" << std::endl;
                    //exit(0);
                    response(*client_it);
                    if (client_it->getState() == DONE)
                    {
                        client_it = clients.erase(client_it);
                        close(client_it->getClientSocket());
                        FD_CLR((*client_it).getClientSocket(), &writeSet);
                    }
                    continue;
                    // // (*it1).getClientSocket() = -1;
                    // // if (it1 != clients.end()) {
                    // //     clients.erase(it1);
                    // //     it1--;
                }
                ++client_it;
            }
        }
    }
}
