/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 19:01:01 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/14 12:45:38 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string serv, std::string _port)
{
    serverName = serv;
    port = _port;
}

Server::~Server()
{
    
}

void Server::CreateSocket()
{
    memset(&hint, 0, sizeof(hint));
    
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    int yes = 1;
    
    if (getaddrinfo(serverName.c_str(), port.c_str(), &hint, &res) != 0)
    {
        std::cerr << "getaddrinfo() failed" << std::endl;
        return;
    }

    server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_socket == -1) {
        perror("socket");
        freeaddrinfo(res);
        return;
    }

    // if ((server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
    // {
    //     std::cerr << "socket() failed" << std::endl;
    //     exit(1);
    // }
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Failed to set socket to non-blocking mode" << std::endl;
        close(server_socket);
        freeaddrinfo(res);
        return;
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    {
        std::cout << "setsocket failed" << std::endl;
        close(server_socket);
        freeaddrinfo(res);
        return;
    }
    
    if (bind(server_socket, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("bind");
        close(server_socket);
        freeaddrinfo(res);
        return;
    }
    freeaddrinfo(res);
    
    if (listen(server_socket, 400) == -1)
    {
        std::cerr << "listen() failed" << std::endl;
        close(server_socket);
        return;
    }
}