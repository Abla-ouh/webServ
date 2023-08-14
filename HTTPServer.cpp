/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:20:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/14 15:00:48 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "HTTPServer.hpp"


HTTPServer::HTTPServer() {
    
    servers.push_back(Server("localhost", "9090"));
   // servers.push_back(Server("localhost", "9090"));
}

HTTPServer::~HTTPServer()
{
    
}

void HTTPServer::createConnections()
{
    //  initializing the socket for each server
    for (size_t i = 0; i < servers.size(); i++)
    {
        servers[i].CreateSocket();
    }
}

std::string intToString(int number)
{
    std::stringstream ss;
    ss << number;
    std::string strNumber = ss.str();
    return strNumber;
}

void HTTPServer::readFromFile(std::string file, std::string &str)
{
    std::ifstream inputFile(file.c_str());
    if (!inputFile) {
        std::cerr << "Error opening file" << std::endl;
        exit(1);
    }
    std::string line;
    while (getline(inputFile, line))
        str += line + "\n";
    inputFile.close();
}

void HTTPServer::addClient(int clientSocket)
{
    clients.push_back(Client(clientSocket));
    std::cout << " Accepted new client connection." << std::endl;
}

void HTTPServer::removeClient(int clientSocket)
{
    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it).client_socket == clientSocket)
        {
            close(clientSocket);
            clients.erase(it);
            std::cout << " Client Disconnected." << std::endl;
            break;
        }
    }
}

bool isValid_URI_Char(char c) {
    // List of allowed characters in a URI (add more as needed)
    const std::string allowedCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                          "abcdefghijklmnopqrstuvwxyz"
                                          "0123456789"
                                          "-._~:/?#[]@!$&'()*+,;=%";

    // Check if the character is in the list of allowed characters
    return allowedCharacters.find(c) != std::string::npos;
}

void HTTPServer::sendErrorResponse(int clientSocket, const std::string& statusLine)
{
    std::string responseBody;
    readFromFile("error.html", responseBody);
    //std::string responseBody = "<html><body><h1>" + statusLine + "</h1></body></html>";
    std::string responseHeader = "HTTP/1.1 " + statusLine + "\r\n";
    responseHeader += "Content-Type: text/html\r\n";
    responseHeader += "Content-Length: " + intToString(responseBody.length()) + "\r\n";
    responseHeader += "\r\n";
    responseHeader += responseBody;

    ssize_t bytesSent = send(clientSocket, responseHeader.c_str(), responseHeader.length(), 0);
    if (bytesSent == -1) {
        std::cerr << "Failed to send error response." << std::endl;
    } else {
        std::cout << "Sent " << bytesSent << " bytes of error response." << std::endl;
    }
    close(clientSocket);
}

void HTTPServer::handleRequest(int clientSocket)
{
    char data[1024];
    int rd = read(clientSocket, data, sizeof(data));
    
    if (rd < 0) {
        std::cerr << "An error has occurred during reading request from a client" << std::endl;
    } else if (rd == 0) {
        // Client has closed the connection
        std::cout << "Client has closed the connection" << std::endl;
        // Remove the client from the list
        removeClient(clientSocket);
    } else {
        // Process the request normally
        // std::cout << "Received data from client: " << data << std::endl;
        std::string httpRequest(data, rd);
        Request request(httpRequest);
        
        
        std::string method = request.getMethod();
        std::string uri = request.getURI();
        std::string version = request.getVersion();
        std::string query = request.getQuery();

        std::string userAgent = request.getHeader("User-Agent");
        std::string contentType = request.getHeader("Content-Type");
        std::string transferEncoding = request.getHeader("Transfer-Encoding");
        std::string contentLengthStr = request.getHeader("Content-Length");

        if (!transferEncoding.empty() && transferEncoding != "chunked")
        {
            sendErrorResponse(clientSocket, "501 Not Implemented");
            return;
        }

        if (method == "POST" && contentLengthStr.empty() && transferEncoding.empty()) {
            sendErrorResponse(clientSocket, "400 Bad Request");
            return;
        }
    
        for (size_t i = 0; i < uri.length(); i++) {
            if (!isValid_URI_Char(uri[i])) {
                sendErrorResponse(clientSocket, "400 Bad Request");
                return;
            }
        }
        if (uri.length() > 2048) {
            sendErrorResponse(clientSocket, "414 Request-URI Too Long");
            return;
        }
        
        //Until i get the body size from config file 
        // if (!contentLengthStr.empty()) {
        //     size_t contentLength = atoi(contentLengthStr.c_str());
        //     size_t maxBodySize = getConfiguredMaxBodySize();
        //     if (contentLength > maxBodySize) {
        //         sendErrorResponse(clientSocket, "413 Request Entity Too Large");
        //         return;
        //     }
        // }

        if (method == "GET"){ std::cout << "GET"<< std::endl;}
        else if (method == "POST"){ std::cout << "POST"<< std::endl;}
        else if (method == "DELETE"){ std::cout << "DELETE"<< std::endl;}
        else
        {
            perror("error");
            std::cerr << "Unsupported HTTP method: " << method << std::endl;
            //close(clientSocket);  
        }
        /************************ PRINT****************************/
        std::cout << "Method: " << request.getMethod() << std::endl;
        std::cout << "URI: " << request.getURI() << std::endl;
        std::cout << "Version: " << request.getVersion() << std::endl;
        std::cout << "Query: " << request.getQuery() << std::endl;

        // Log the headers
        std::cout << "Headers:" << std::endl;
        const std::string headersToLog[] = {
            "User-Agent", "Content-Type", "Transfer-Encoding", "Content-Length" // Add other headers you're interested in
        };
        for (size_t i = 0; i < sizeof(headersToLog) / sizeof(headersToLog[0]); ++i) {
            const std::string& headerValue = request.getHeader(headersToLog[i]);
            if (!headerValue.empty()) {
                std::cout << headersToLog[i] << ": " << headerValue << std::endl;
            }
        }
        std::cout << "-------------------------------------------\n";
        /************************ PRINT****************************/   
    }
}

void HTTPServer::sendResponse(int clientSocket)
{
    std::string responseBody;
    readFromFile("index.html", responseBody);
    std::string responseHeader = "HTTP/1.1 200 OK\r\n";
    responseHeader += "Content-Type: text/html\r\n";
    responseHeader += "Content-Length: " + intToString(responseBody.length()) + "\r\n";
    responseHeader += "\r\n";
    responseHeader += responseBody;

    ssize_t bytesSent = send(clientSocket, responseHeader.c_str(), responseHeader.length(), 0);
    if (bytesSent == -1) {
        std::cerr << "Failed to send response header." << std::endl;
    } else {
        std::cout << "Sent " << bytesSent << " bytes of response header." << std::endl;
    }
    close(clientSocket);
}

void acceptNewClient(std::vector<Server>& servers, std::vector<Client>& clients, fd_set& rd)
{
    std::vector<Server>::iterator it = servers.begin();
    int serverIndex = 0; 
    while (it != servers.end())
    {
        if (FD_ISSET((*it).server_socket, &rd))
        {
            Client newClient;
            newClient.client_socket = accept((*it).server_socket, NULL, NULL);
            if (newClient.client_socket == -1)
            {
                std::cerr << "Failed to accept client connection" << std::endl;
                continue;
            }

            // Set the client socket to non-blocking mode
            fcntl(newClient.client_socket, F_SETFL, O_NONBLOCK);

            newClient.server_id = serverIndex;
            clients.push_back(newClient);
        }
        it++;
        serverIndex++;
    }
}


void HTTPServer::start()
{
    fd_set readSet, writeSet;
    int maxSocket;

    signal(SIGINT, SIG_IGN);

    createConnections();
    while(true)
    {
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        maxSocket = -1;

        std::vector<Server>::iterator it = servers.begin();
        while (it != servers.end())
        {
            //write ??
            FD_SET((*it).server_socket, &readSet);
            if ((*it).server_socket > maxSocket)
                maxSocket = (*it).server_socket;
            it++;
        }

        std::vector<Client>::iterator it1 = clients.begin();
        while (it1 != clients.end())
        {
            FD_SET((*it1).client_socket, &readSet);
            FD_SET((*it1).client_socket, &writeSet);
            if ((*it1).client_socket > maxSocket)
                maxSocket = (*it1).client_socket;
            it1++;
        }
        
        if (select(maxSocket + 1, &readSet, &writeSet, NULL, NULL) == -1)
            std::cerr << "Error in Select !" << std::endl;
        
        else
        {
            acceptNewClient(servers, clients, readSet);
            // Handle client requests and send responses
            std::vector<Client>::iterator it = clients.begin();
            while (it != clients.end())
            {
                if (FD_ISSET((*it).client_socket, &readSet))
                {
                    //std::cout<< "TESTTTTT\n";
                    fcntl((*it).client_socket, F_SETFL, O_NONBLOCK);
                    handleRequest((*it).client_socket);
                }

                if (FD_ISSET((*it).client_socket, &writeSet))
                {
                    sendResponse((*it).client_socket);
                    it = clients.erase(it);
                    continue;
                    // (*it1).client_socket = -1;
                    // if (it1 != clients.end()) {
                    //     clients.erase(it1);
                    //     it1--;
                }
                ++it;
            }

        }
    }
}