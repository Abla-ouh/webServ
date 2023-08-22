/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:20:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/22 18:36:08 by abouhaga         ###   ########.fr       */
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


void parseRequestBody(Client &client/*, fd_set &writeSet*/)
{
    Request &request = client.getRequest();
    std::string transferEncoding = request.getHeader("Transfer-Encoding");
    std::string contentLengthStr = request.getHeader("Content-Length");
    ssize_t bytesRead;

    if (!transferEncoding.empty() && !contentLengthStr.empty())
    {
        client.setStatus(400); // Bad Request
        return;
    }
    // open the file in both "write" mode and "binary" mode simultaneously
    std::ofstream bodyFile("request_body", std::ios::out | std::ios::binary);
    if (!bodyFile.is_open())
    {
        client.setStatus(500);
        return;
    }
    
    if (!transferEncoding.empty() && transferEncoding == "chunked")
    {
        while (true)
        {
            std::string chunkSizeStr;
            char c;
            // Read the chunk size as a hexadecimal string
            while (read(client.getClientSocket(), &c, 1) == 1)
            {
                if (c == '\r')
                    continue;
                if (c == '\n')
                    break;
                chunkSizeStr += c;
            }
            // Exit the loop if no more chunks or invalid chunk size
            if (chunkSizeStr.empty())
                break;
            //to int
            size_t chunkSize = strtoul(chunkSizeStr.c_str(), NULL, 16);
            if (chunkSize == 0)
                break;
            //Allocate mem for chunk
            char *chunkData = new char[chunkSize];
            bytesRead = read(client.getClientSocket(), chunkData, chunkSize);
            if (bytesRead == static_cast<ssize_t>(chunkSize))
            {
                bodyFile.write(chunkData, chunkSize);
                delete[] chunkData;

                // Read at the end of the chunk
                char crlf[2];
                if (read(client.getClientSocket(), crlf, 2) != 2)
                    break;
            }
            else
            {
                //clean up
                delete[] chunkData;
                std::cerr << "Error reading request body" << std::endl;
                break;
            }
        }
        client.setBodyReady(true); // Mark body as ready 
        client.setParsedRequestBodyFilename("request_body");
    }

    else if (!contentLengthStr.empty())
    {
        // later
    }

    else
    {
        // Read char by char
        
    }
    bodyFile.close();
    //FD_SET(client.getClientSocket(), &writeSet);
}

void HTTPServer::handleRequest(Client &client, fd_set &writeSet)
{
    Request &request = client.getRequest();
    if (client.currentState == HEADER_READING)
    {
        char data[1024];
        int rd = read(client.getClientSocket(), data, sizeof(data));
        
        if (rd < 0)
            std::cerr << "An error has occurred during reading request from a client" << std::endl;
        else if (rd == 0)
        {
            // Client has closed the connection
            std::cout << "Client has closed the connection" << std::endl;
            // Remove the client from the list
            removeClient(client.getClientSocket());
        }
        else
        {
            // Process the request normally
            // std::cout << "Received data from client: " << data << std::endl;
            std::string httpRequest(data, rd);
            request.initRequest(httpRequest);
            
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
                client.setStatus(501);
                //sendErrorResponse(client.getClientSocket(), "501 Not Implemented");
                return;
            }

            if (method == "POST" && contentLengthStr.empty() && transferEncoding.empty()) {
                client.setStatus(400);
                //sendErrorResponse(client.getClientSocket(), "400 Bad Request");
                return;
            }
        
            for (size_t i = 0; i < uri.length(); i++)
            {
                if (!isValid_URI_Char(uri[i]))
                {
                    client.setStatus(400);
                    //sendErrorResponse(client.getClientSocket(), "400 Bad Request");
                    return;
                }
            }
            if (uri.length() > 2048)
            {
                client.setStatus(414);
                //sendErrorResponse(client.getClientSocket(), "414 Request-URI Too Long");
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
        }
        //if (request.getMethod() == "GET" ||  request.getMethod() == "DELETE")
        client.currentState = BODY_READING;
        return;
    }
    if (client.getCurrentState() == BODY_READING && request.getMethod() == "POST")
        parseRequestBody(client);
    FD_SET(client.getClientSocket(), &writeSet);
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

void acceptNewClient(std::vector<server>& servers, std::vector<Client>& clients, fd_set& rd)
{
    Client newClient;
    
    std::vector<server>::iterator it = servers.begin();
    int serverIndex = 0; 
    while (it != servers.end())
    {
        if (FD_ISSET((*it).getServerSocket(), &rd))
        {
            newClient.setClientSocket(accept((*it).getServerSocket(), NULL, NULL));
            if (newClient.getClientSocket() == -1)
            {
                std::cerr << "Failed to accept client connection" << std::endl;
                continue;
            }

            // Set the client socket to non-blocking mode
            fcntl(newClient.getClientSocket(), F_SETFL, O_NONBLOCK);

            newClient.setServer(*it);
            clients.push_back(newClient); // New client will be destoyed but its client_socket will keep the return from accept
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

        std::vector<server>::iterator it = servers.begin();
        while (it != servers.end())
        {
            //write ??
            FD_SET((*it).getServerSocket(), &readSet);
            if ((*it).getServerSocket() > maxSocket)
                maxSocket = (*it).getServerSocket();
            it++;
        }
        
        
        std::vector<Client>::iterator it1 = clients.begin();
        while (it1 != clients.end())
        {
            FD_SET((*it1).getClientSocket(), &readSet); // ghi tread tansali l9raya w ndir write fd
            if ((*it1).getClientSocket() > maxSocket)
                maxSocket = (*it1).getClientSocket();
            it1++;
        }
        
        if (select(maxSocket + 1, &readSet, &writeSet, NULL, NULL) == -1)
            std::cerr << "Error in Select !" << std::endl;
        
        else
        {
            acceptNewClient(servers, clients, readSet); // each accepted client with its own virtual server
            // Handle client requests and send responses
            std::vector<Client>::iterator it = clients.begin();
            while (it != clients.end())
            {
                if (FD_ISSET((*it).getClientSocket(), &readSet))
                {
                    //std::cout<< "TESTTTTT\n";
                    fcntl((*it).getClientSocket(), F_SETFL, O_NONBLOCK);
                    handleRequest(*it, writeSet);
                }

                if (FD_ISSET((*it).getClientSocket(), &writeSet))
                {
                    response(*it);
                    // sendResponse((*it).getClientSocket());
                    it = clients.erase(it);
                    continue;
                    // (*it1).getClientSocket() = -1;
                    // if (it1 != clients.end()) {
                    //     clients.erase(it1);
                    //     it1--;
                }
                ++it;
            }

        }
    }
}
