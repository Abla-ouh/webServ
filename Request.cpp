/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybel-hac <ybel-hac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 16:57:48 by abouhaga          #+#    #+#             */
/*   Updated: 2023/09/05 15:13:28 by ybel-hac         ###   ########.fr       */
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

void Request::parseCookies(const std::string& cookieHeader)
{
    size_t i = 0;
    while (i < cookieHeader.length())
    {
        size_t j = cookieHeader.find(';', i);
        if (j == std::string::npos)
            j = cookieHeader.length();
        std::string cookiePair = cookieHeader.substr(i, j - i); 
        size_t equals = cookiePair.find('=');
        if (equals != std::string::npos)
        {
            std::string cookieName = cookiePair.substr(0, equals);
            std::string cookieValue = cookiePair.substr(equals + 1);
            // leading and trailing whitespace
            size_t firstNonSpace = cookieName.find_first_not_of(" \t"); //first character that is not a space or a tab
            size_t lastNonSpace = cookieName.find_last_not_of(" \t");
            if (firstNonSpace != std::string::npos && lastNonSpace != std::string::npos) {
                cookieName = cookieName.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1); 
            }
            firstNonSpace = cookieValue.find_first_not_of(" \t");
            lastNonSpace = cookieValue.find_last_not_of(" \t");
            if (firstNonSpace != std::string::npos && lastNonSpace != std::string::npos) {
                cookieValue = cookieValue.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
            }
            cookies[cookieName] = cookieValue;
        }
        i = j + 1;
    }
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
    std::string cookieHeader = getHeader("Cookie");
    if (!cookieHeader.empty()) {
        parseCookies(cookieHeader); // Parse cookies from the "Cookie" header
    }
    // std::cout << "method: " << method << std::endl;
    // std::cout << "uri: " << uri << std::endl;
    // std::cout << "query: " << query << std::endl;
    // std::cout << "version: " << version << std::endl;
    // std::cout << "headers: " << std::endl;
    // for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
    //     std::cout << it->first << ": " << it->second << std::endl;
    // }
    // std::cout << "cookies: " << std::endl;
    // for (std::map<std::string, std::string>::const_iterator it = cookies.begin(); it != cookies.end(); ++it) {
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
        return it->second;
    }
    static std::string emptyHeader = ""; // there's no need to create a new instance of it every time the function is called
    return emptyHeader; //empty string 
}

void Request::parseHeaders(const std::string& headersBlock)
{
    string    headersBlock2 = headersBlock;

    headersBlock2.append("\r\n\r\n");
    while (headersBlock2.find("\r\n\r\n") != string::npos)
    {
        string line = headersBlock2.substr(0, headersBlock2.find("\r\n"));
        headersBlock2.erase(0, headersBlock2.find("\r\n") + 2);
        string key = line.substr(0, line.find(":"));
        string value = line.substr(line.find(":") + 2);
        headers[key] = value;
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
				if (client.file != -1)
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
		client.bodyReaded += write_size;
		if (client.bodyReaded >= atoll(client.getServer().getclient_max_body_size().c_str()))
		{
			client.setStatus(413);
			if (client.file != -1)
				close(client.file);
            client.isBodyReady = true;
            client.ready = true;
			FD_CLR(client.getClientSocket(), &readSet);
            FD_SET(client.getClientSocket(), &writeSet);
			return;
		}
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

int HTTPServer::handleRequest(std::vector<Client>::iterator &client_it, fd_set &writeSet, fd_set &readSet, int &maxSocket)
{
    Client &client = *client_it;
    Request &request = client.getRequest();
    char data[20240];
    memset(data, '\0', 20240);
    int rd = recv(client.getClientSocket(), data, 20240, 0);

    if (rd < 0)
    {
		if (client.file != -1)
			close(client.file);
        std::cout << "Client has closed the connection" << std::endl;
        FD_CLR(client.getClientSocket(), &readSet);
        removeClient(client_it, maxSocket);
		cout << PURPLE << "handleRequest CLIENT Dropped: " << client_it->getClientSocket() << WHITE "\n";
        return 0;
    }
    if (!rd)
    {
		if (client.file != -1)
			close(client.file);
        FD_CLR(client.getClientSocket(), &readSet);
        FD_SET(client.getClientSocket(), &writeSet);
        return 1;
    }

    if (client.currentState == HEADER_READING)
    {

        if (!client.already_checked)
        {
            client._return = is_carriage(std::string(data), client);
            if (client.readyToParse)
            {
                client.header = std::string(data).substr(0, client._return);

                request.initRequest(client.header);
                if (!RequestErrors(request, client))
                {
					if (client.file != -1)
						close(client.file);
                    FD_CLR(client.getClientSocket(), &readSet);
                    FD_SET(client.getClientSocket(), &writeSet);
					return (1);
                }
                client.bodyPos = client._return + 4;
                client.bodyChunked = isChuncked(client.header);
                client.firstTime = 1;
                client.already_checked = true;
            }
            else
            {
                std::cout << "Client has closed the connection" << std::endl;
                FD_CLR(client.getClientSocket(), &readSet);
                removeClient(client_it, maxSocket);
				cout << PURPLE << "CLIENT Dropped: " << client.getClientSocket() << WHITE "\n";
                return 0;
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
            return 1;
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
                if ((size_t)std::atoi(request.getHeader("Content-Length").c_str()) < rd - client.bodyPos)
                    write(client.file, holder, std::atoi(request.getHeader("Content-Length").c_str()));
                else
                    write(client.file, holder, rd - client.bodyPos);
                if (getFileSize(client.file_name) == (size_t)std::atoi(request.getHeader("Content-Length").c_str())) {
                    close(client.file);
                    client.isBodyReady = true;
                    client.ready = true;
                    FD_CLR(client.getClientSocket(), &readSet);
                    FD_SET(client.getClientSocket(), &writeSet);
					return 1;
                }
            }
            else
            {
				if (client.file != -1)
					close(client.file);
                client.isBodyReady = true;
                client.ready = true;
				FD_CLR(client.getClientSocket(), &readSet);
                FD_SET(client.getClientSocket(), &writeSet);
				return 1;
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
					if (client.file != -1)
						close(client.file);
                    client.isBodyReady = true;
                    client.ready = true;
                    FD_CLR(client.getClientSocket(), &readSet);
                    FD_SET(client.getClientSocket(), &writeSet);
					return 1;
                }
            }
            else
            {
				if (client.file != -1)
					close(client.file);
                client.isBodyReady = true;
                client.ready = true;
                FD_CLR(client.getClientSocket(), &readSet);
                FD_SET(client.getClientSocket(), &writeSet);
				return 1;
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
    return 1;
}