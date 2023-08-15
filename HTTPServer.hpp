/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:55:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/15 19:23:57 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "./utils/configParse/serverClass.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include "./utils/configParse/configFile.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <errno.h>
#include <stdio.h>
#include <fstream>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>

class HTTPServer {
    public:
        HTTPServer(configFile &obj);
        ~HTTPServer();
        void createConnections();
        void start();
        std::vector<server> servers;
        std::vector<Client> clients;

    private:
    
        void readFromFile(std::string file, std::string &str);
        void addClient(int clientSocket);
        void removeClient(int clientSocket);
        void handleRequest(int clientSocket, server servers);
        void sendResponse(int clientSocket);
        void sendErrorResponse(int clientSocket, const std::string& statusLine);
        std::string get_resource_type(const std::string& uri);
        void handleDeleteRequest(int clientSocket, const std::string& uri, server servers);

};

#endif
