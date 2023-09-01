/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybel-hac <ybel-hac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 16:57:48 by abouhaga          #+#    #+#             */
/*   Updated: 2023/09/01 10:30:36 by ybel-hac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "HTTPServer.hpp"


std::string getQuerySting(std::string &URI)
{
    if(URI.find("?") != std::string::npos)
    {
        std::string str;
        str = URI.substr(URI.find("?"));
        URI = URI.substr(0, URI.find("?"));
        return str;
    }
    //if not found ?
    return "";
}

Request::Request()
{
    
}

void Request::initRequest(const std::string& httpRequestHeader)
{
    size_t pos = httpRequestHeader.find(" ");
    method = httpRequestHeader.substr(0, pos);
    std::string remainingRequest = httpRequestHeader.substr(pos + 1);
    
    pos = remainingRequest.find(" ");
    uri = remainingRequest.substr(0, pos);
    remainingRequest = remainingRequest.substr(pos + 1);
    
    query = getQuerySting(uri);
    
    pos = remainingRequest.find("\r\n");
    version = remainingRequest.substr(0, pos);
    
    pos += 2; // Move to the beginning of headers
    std::string headersBlock = remainingRequest.substr(pos);
    
    parseHeaders(headersBlock);
    // std::cout << "method: " << method << std::endl;
    // std::cout << "uri: " << uri << std::endl;
    // std::cout << "query: " << query << std::endl;
    // std::cout << "version: " << version << std::endl;
    // std::cout << "headers: " << std::endl;
    // for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
    //     std::cout << it->first << ": " << it->second << std::endl;
    // }
}


// read-only access to the object's data
std::string& Request::getMethod()  {
    return method;
}

std::string& Request::getURI()  {
    return uri;
}

std::string& Request::getQuery()  {
    return query;
}

std::string& Request::getVersion()  {
    return version;
}

std::string& Request::getHeader(const std::string& key) {
    std::map<std::string, std::string>::iterator it = headers.find(key);
    if (it != headers.end()) {
        return it->second; //returns the associated value
    }
    static std::string emptyHeader = ""; // there's no need to create a new instance of it every time the function is called
    return emptyHeader; //empty string 
}

void Request::parseHeaders(const std::string& headersBlock)
{
    size_t pos = 0;
    while (pos < headersBlock.size())
    {
        size_t lineEnd = headersBlock.find("\r\n", pos);
        if (lineEnd == std::string::npos) //no more \r\n sequences are found
            break;
        std::string headerLine = headersBlock.substr(pos, lineEnd - pos); //Extract the current header
        size_t colonPos = headerLine.find(": ");
        if (colonPos != std::string::npos) { //colon separator is found
            std::string key = headerLine.substr(0, colonPos);
            std::string value = headerLine.substr(colonPos + 2); // +2 skip the colon and space
            headers[key] = value; //store key-value pair
        }
        pos = lineEnd + 2; //move nextLine + skip \r\n
    }
}

bool Request::isValid_URI_Char(char c) {
    // List of allowed characters in a URI (add more as needed)
    const std::string allowedCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                          "abcdefghijklmnopqrstuvwxyz"
                                          "0123456789"
                                          "-._~:/?#[]@!$&'()*+,;=%";

    // Check if the character is in the list of allowed characters
    return allowedCharacters.find(c) != std::string::npos;
}


bool RequestErrors(Request &request, Client &client)
{
    std::string method = request.getMethod();
    std::string uri = request.getURI();
    std::string version = request.getVersion();
    std::string query = request.getQuery();

    std::string userAgent = request.getHeader("User-Agent");
    std::string contentType = request.getHeader("Content-Type");
    std::string transferEncoding = request.getHeader("Transfer-Encoding");
    std::string contentLengthStr = request.getHeader("Content-Length");
  
    if (method != "GET" && method != "POST" && method != "DELETE")
    {
        client.setStatus(501);
        return 0;
    }
    if (!transferEncoding.empty() && transferEncoding != "chunked")
    {
        client.setStatus(501);
        return  0;
    }

    if (method == "POST" && contentLengthStr.empty() && transferEncoding.empty()) {
        client.setStatus(400);
        return 0;
    }
        
    for (size_t i = 0; i < uri.length(); i++)
    {
        if (!request.isValid_URI_Char(uri[i]))
        {
            client.setStatus(400);
            return 0;
        }
    }
    if (uri.length() > 2048)
    {
        client.setStatus(414);
        return 0;
    }
    if (request.getVersion() != "HTTP/1.1" && request.getVersion() != "HTTP/1.0") {
        client.setStatus(505);
        return 0;
    }
    if (!contentLengthStr.empty() && request.getMethod() == "POST") {
        size_t contentLength = atoi(contentLengthStr.c_str());
        size_t maxBodySize = atoi(client.getServer().getclient_max_body_size().c_str());
        if (contentLength > maxBodySize) {
            client.setStatus(413);
            return 0;
        }
    }

    return (1);
}


bool findHex(Client &client)
{
    char *endOfChunkSize;
    int chunkSizeLength;

    // Check if "\r\n" is present in the hex_str buffer
    if (!memmem(client.hexBuff, client.hex_len, "\r\n", 2)) //first
        return false;
    // Find position of the second "\r\n" after the first one
    endOfChunkSize = (char *)memmem(client.hexBuff + 2, client.hex_len, "\r\n", 2);
    if (!endOfChunkSize)
        return false;

    chunkSizeLength = endOfChunkSize - (client.hexBuff  + 2);
	memcpy(client.hexTempBuff, client.hexBuff + 2, chunkSizeLength);
    memset(client.hexBuff, 0, 20);
    memcpy(client.hexBuff, client.hexTempBuff, chunkSizeLength);
    client.waiting_for_chunk_size = true;
    
    return true;
    
}

template<typename T>
T custom_min(T a, T b) {
    return a < b ? a : b;
}


void ft_chunked(Client &client, const char *data, int b_length, fd_set &writeSet, fd_set &readSet)
{
    int rd_times = 0;
    client.waiting_for_chunk_size = false;
    int write_size;

    while (rd_times < b_length)
    {
        if (!client.chunk_size)
        {
            while (rd_times < b_length)
            {
                client.hexBuff[client.hex_len++] = data[rd_times++];
                if (findHex(client))
                    break;
            }

            // If chunk size indicator not found, wait for more data
            if (!client.waiting_for_chunk_size)
                break;

            char chunkSizeBuffer[10];
            memcpy(chunkSizeBuffer, client.hexBuff, client.hex_len);
            chunkSizeBuffer[client.hex_len] = '\0';
            client.chunk_size = strtoul(chunkSizeBuffer, NULL, 16);

            client.hex_len = 0;
            memset(client.hexBuff, 0, 20);

            if (!client.chunk_size)
            {
                close(client.file);
                client.isBodyReady = true;
                client.ready = true;
				FD_CLR(client.getClientSocket(), &readSet);
                FD_SET(client.getClientSocket(), &writeSet);
                break;
            }
        }

        write_size = custom_min<size_t>(client.chunk_size, b_length - rd_times);
        write(client.file, data + rd_times, write_size);

        client.chunk_size -= write_size;
        rd_times += write_size;
    }
}

bool isChuncked(std::string request)
{
    std::vector<std::string> output;
	std::string token;
	std::stringstream ss(request);
	while (std::getline(ss, token, '\n')) {
		output.push_back(token);
	}
	std::vector<std::string>::iterator it;
	for (it = output.begin(); it != output.end(); it++){
		std::string tmp = *it;
		if (tmp.find("Transfer-Encoding: chunked") != std::string::npos) {
			return true;
		}
	}
	return false;
}

void HTTPServer::handleRequest(Client &client, fd_set &writeSet, fd_set &readSet)
{
    Request &request = client.getRequest();
    char data[20240];
	memset(data,'\0', 20240);
    int rd  = recv(client.getClientSocket(), data, 20240, 0);
    if (client.currentState == HEADER_READING)
    {
        if (rd <= 0)
        {
            std::cout << "Client has closed the connection" << std::endl;
            removeClient(client.getClientSocket());
        }

        if (!client.already_checked)
        {
            client._return = is_carriage(std::string(data), client);
            if (client.readyToParse)
            {
                client.header = std::string(data).substr(0, client._return);

                request.initRequest(client.header);
                if (!RequestErrors(request, client))
                {
                    FD_CLR(client.getClientSocket(), &readSet);
                    FD_SET(client.getClientSocket(), &writeSet);
                }
                client.bodyPos = client._return + 4;
                client.bodyChunked = isChuncked(client.header);
                client.firstTime = 1;
                client.already_checked = true;
            }
            else
            {
                removeClient(client.getClientSocket());
                // close(client.fd_client);
                //FD_CLR(client.getClientSocket(), &readSet);
                return;
            }
	    }
        if (request.getMethod() == "POST")
        {
            client.currentState = BODY_READING;
			client.file = open(client.file_name.c_str(), O_CREAT | O_RDWR | O_APPEND, 0644);
			client.uploadedInFile = open(client.file_name.c_str(), O_CREAT | O_RDWR | O_APPEND, 0644);
        }
        else
        {
            FD_CLR(client.getClientSocket(), &readSet);  
            FD_SET(client.getClientSocket(), &writeSet);
            return ;
        }
    }
    if (!client.bodyChunked && client.getCurrentState() == BODY_READING)
    {
        if (client.firstTime)
        {
            char *holder;
            holder = substr_no_null(data, client.bodyPos, rd, rd);

            if ((getFileSize(client.file_name) < (size_t)std::atoi(request.getHeader("Content-Length").c_str())) && holder != NULL && std::atoi(request.getHeader("Content-Length").c_str()) != 0)
            {
                std::cout << "content lenght : " << std::atoi(request.getHeader("Content-Length").c_str()) << std::endl;
                std::cout << "readBytes: " << rd - client.bodyPos << std::endl;
                std::cout << "getFileSize: " << getFileSize(client.file_name) << std::endl;
                if ((size_t)std::atoi(request.getHeader("Content-Length").c_str()) < rd - client.bodyPos)
                    write(client.file, holder, std::atoi(request.getHeader("Content-Length").c_str()));
                else
                    write(client.file, holder, rd - client.bodyPos);
                // write(client.file, holder, rd - client.bodyPos);
                std::cout << "getFileSize: " << getFileSize(client.file_name) << std::endl;
                std::cout << "holder: " << holder << std::endl;
                if (getFileSize(client.file_name) == (size_t)std::atoi(request.getHeader("Content-Length").c_str())) {
                    close(client.file);
                    client.isBodyReady = true;
                    client.ready = true;
                    FD_CLR(client.getClientSocket(), &readSet);
                    FD_SET(client.getClientSocket(), &writeSet);
                }
            }
            else
            {
                close(client.file);
                client.isBodyReady = true;
                client.ready = true;
				FD_CLR(client.getClientSocket(), &readSet);
                FD_SET(client.getClientSocket(), &writeSet);
            }
            client.firstTime = 0;
        }
        else
        {
            if (getFileSize(client.file_name) < (size_t)std::atoi(request.getHeader("Content-Length").c_str()))
            {
                write(client.file, data, rd);
                if (getFileSize(client.file_name) == (size_t)std::atoi(request.getHeader("Content-Length").c_str()))
                {
                    close(client.file);
                    client.isBodyReady = true;
                    client.ready = true;
                    FD_CLR(client.getClientSocket(), &readSet);
                    FD_SET(client.getClientSocket(), &writeSet);
                }
            }
            else
            {
                close(client.file);
                client.isBodyReady = true;
                client.ready = true;
                FD_CLR(client.getClientSocket(), &readSet);
                FD_SET(client.getClientSocket(), &writeSet);
            }
        }
        
	}
  
    else if (client.bodyChunked && client.getCurrentState() == BODY_READING)
    {
        if (client.firstTime)
        {
            client.bodyPos -= 2;
            char *holder;
            holder = substr_no_null(data, client.bodyPos, rd, rd);
			ft_chunked(client, holder, rd - client.bodyPos, writeSet, readSet);
            delete []holder;
            client.firstTime = 0;
        }
        else
            ft_chunked(client, data, rd, writeSet, readSet);
    }
}