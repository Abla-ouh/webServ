/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelkhei <ebelkhei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 16:57:48 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/23 17:06:05 by ebelkhei         ###   ########.fr       */
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

void Request::initRequest(const std::string& httpRequest)
{
    size_t pos = httpRequest.find(" ");
    method = httpRequest.substr(0, pos);
    std::string remainingRequest = httpRequest.substr(pos + 1);
    
    pos = remainingRequest.find(" ");
    uri = remainingRequest.substr(0, pos);
    remainingRequest = remainingRequest.substr(pos + 1);
    
    query = getQuerySting(uri);
    
    pos = remainingRequest.find("\r\n");
    version = remainingRequest.substr(0, pos);
    
    pos += 2; // Move to the beginning of headers
    std::string headersBlock = remainingRequest.substr(pos);
    
    parseHeaders(headersBlock);
}


// read-only access to the object's data
const std::string& Request::getMethod() const {
    return method;
}

std::string& Request::getURI() {
    return uri;
}

const std::string& Request::getQuery() const {
    return query;
}

const std::string& Request::getVersion() const {
    return version;
}

const std::string& Request::getHeader(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it != headers.end()) {
        return it->second; //returns the associated value
    }
    static const std::string emptyHeader = ""; // there's no need to create a new instance of it every time the function is called
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