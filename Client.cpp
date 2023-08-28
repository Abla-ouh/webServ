/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:35:35 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/28 12:46:30 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "HTTPServer.hpp"


std::string RandomString(int length) {
    const char chars[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const int charsSize = sizeof(chars) - 1;
    std::string randomString(length, ' ');

    std::srand(time(0)); // Seed the random number generator

    for (int i = 0; i < length; ++i) {
        randomString[i] = chars[std::rand() % charsSize];
    }

    return randomString;
}

Client::Client() : currentState(HEADER_READING)
{
    this->locations = _server.getLocation();
    this->status = 0; 
    this->state = BUILDING;
    memset(data, 0, 8000);
    std::string p = "file_";
	file_name = p.append(RandomString(6));
    //file_name = "request_body";
    file = open(file_name.c_str(), O_CREAT | O_RDWR | O_APPEND, 0644);
    // std::cout << "fd" << file << std::endl;
    already_checked = false;
	_return = 0;
	readyToParse = false;
	isBodyReady = false;
    firstTime = 0;
    bodyChunked = false;
    ready = false;
	hex_len = 0;
	chunk_size = 0;
    
}

Client::~Client()
{
    
}
