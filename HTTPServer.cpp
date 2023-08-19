/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelkhei <ebelkhei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:20:47 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/19 12:15:33 by ebelkhei         ###   ########.fr       */
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

// void HTTPServer::addClient(int clientSocket)
// {
//     clients.push_back(Client(clientSocket));
//     std::cout << " Accepted new client connection." << std::endl;
// }

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

// std::string HTTPServer::get_resource_type(const std::string& uri)
// {
//     struct stat buf;

//     if (stat(uri.c_str(), &buf) == 0)
//     {
//         if (S_ISREG(buf.st_mode) && access(res, W_OK))
//             return "FILE";
//         else if (S_ISDIR(buf.st_mode))
//             return "DIRE";
//     }
//     return ("INVALID");
// }

// int delete_directory_contents(const std::string& dir_path)
// {
//     DIR* dir = opendir(dir_path.c_str());
//     dirent* entry;

//     if (!dir)
//     {
//         std::cerr << "Error opening directory " << dir_path << std::endl;
//         return 1;
//     }
    
//     while ((entry = readdir(dir)) != NULL)
//     {
//         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
//             continue;

//         std::string entry_path = dir_path + "/" + entry->d_name;
//         if (entry->d_type == DT_DIR)
//         {
//             delete_directory_contents(entry_path);
//             rmdir(entry_path.c_str());
//         }
//         else
//             unlink(entry_path.c_str());
//     }
//     closedir(dir);
//     return 0;
// }


// void HTTPServer::handleDeleteRequest(Client &client, std::vector<server>& servers)
// {
//     std::string full_path = servers.getRoot() + uri;
    
//     std::string rcs_type = get_resource_type(uri);

//     if (rcs_type == "FILE")
//     {
//         // Handle file deletion
//         if (unlink(full_path.c_str()) == 0)
//             // Successfully deleted the file
//             sendResponse(clientSocket);
//         else
//             // Error while deleting the file
//             sendErrorResponse(clientSocket, "500 Internal Server Error");
//     }
//     else if (rcs_type == "DIRE")
//     {
        
//         // Handle directory deletion
//         if (delete_directory_contents(full_path) == 0 && rmdir(full_path.c_str()) == 0)
//             // Successfully deleted the directory
//             sendResponse(clientSocket);
//         else
//             // Error while deleting the directory
//             sendErrorResponse(clientSocket, "500 Internal Server Error");
//     }
//     else
//         // Handle invalid resource type
//         sendErrorResponse(clientSocket, "400 Bad Request");
// }

void HTTPServer::handleRequest(Client &client, fd_set &writeSet)
{

    // if (flag == "H" )
    // {
    //     header_reading()
    // }
    // else if (flag == "B")
    // {
    //     body_reading() -> yt9ra w ydar f file 
    Request &request = client.getRequest();
    char data[1024];
    int rd = read(client.getClientSocket(), data, sizeof(data));
    
    if (rd < 0) {
        std::cerr << "An error has occurred during reading request from a client" << std::endl;
    } else if (rd == 0) {
        // Client has closed the connection
        std::cout << "Client has closed the connection" << std::endl;
        // Remove the client from the list
        removeClient(client.getClientSocket());
    } else {
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
            sendErrorResponse(client.getClientSocket(), "501 Not Implemented");
            return;
        }

        if (method == "POST" && contentLengthStr.empty() && transferEncoding.empty()) {
            sendErrorResponse(client.getClientSocket(), "400 Bad Request");
            return;
        }
    
        for (size_t i = 0; i < uri.length(); i++) {
            if (!isValid_URI_Char(uri[i])) {
                sendErrorResponse(client.getClientSocket(), "400 Bad Request");
                return;
            }
        }
        if (uri.length() > 2048) {
            sendErrorResponse(client.getClientSocket(), "414 Request-URI Too Long");
            return;
        }
        
        FD_SET(client.getClientSocket(), &writeSet);
        //Until i get the body size from config file 
        // if (!contentLengthStr.empty()) {
        //     size_t contentLength = atoi(contentLengthStr.c_str());
        //     size_t maxBodySize = getConfiguredMaxBodySize();
        //     if (contentLength > maxBodySize) {
        //         sendErrorResponse(clientSocket, "413 Request Entity Too Large");
        //         return;
        //     }
        // }

        // if (method == "GET"){ std::cout << "GET"<< std::endl;}
        // else if (method == "POST"){ std::cout << "POST"<< std::endl;}
        // else if (method == "DELETE")
        //     handleDeleteRequest(clientSocket, uri, servers);
        // else
        // {
        //     perror("error");
        //     std::cerr << "Unsupported HTTP method: " << method << std::endl;
        //     //close(clientSocket);  
        // }
        /************************ PRINT****************************/
        // std::cout << "Method: " << request.getMethod() << std::endl;
        // std::cout << "URI: " << request.getURI() << std::endl;
        // std::cout << "Version: " << request.getVersion() << std::endl;
        // std::cout << "Query: " << request.getQuery() << std::endl;

        // // Log the headers
        // std::cout << "Headers:" << std::endl;
        // const std::string headersToLog[] = {
        //     "User-Agent", "Content-Type", "Transfer-Encoding", "Content-Length" // Add other headers you're interested in
        // };
        // for (size_t i = 0; i < sizeof(headersToLog) / sizeof(headersToLog[0]); ++i) {
        //     const std::string& headerValue = request.getHeader(headersToLog[i]);
        //     if (!headerValue.empty()) {
        //         std::cout << headersToLog[i] << ": " << headerValue << std::endl;
        //     }
        // }
        // std::cout << "-------------------------------------------\n";
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
