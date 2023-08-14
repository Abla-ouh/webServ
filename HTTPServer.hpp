/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:55:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/14 12:56:11 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <errno.h>
#include <stdio.h>
#include <fstream>
#include <signal.h>



class HTTPServer {
    public:
        HTTPServer();
        ~HTTPServer();
        void createConnections();
        void start();
        std::vector<Server> servers;
        std::vector<Client> clients;

    private:
    
        void readFromFile(std::string file, std::string &str);
        void addClient(int clientSocket);
        void removeClient(int clientSocket);
        void handleRequest(int clientSocket);
        void sendResponse(int clientSocket);
        void sendErrorResponse(int clientSocket, const std::string& statusLine);
        std::string get_resource_type(const std::string& uri);
};

#endif
