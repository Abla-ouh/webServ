/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelkhei <ebelkhei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:35:35 by abouhaga          #+#    #+#             */
/*   Updated: 2023/09/05 22:54:41 by ebelkhei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	this->err = 1;
	bodyReaded = 0;
    this->locations = _server.getLocation();
    this->status = 0; 
    this->state = BUILDING;
    memset(data, 0, 8000);
    std::string p = "/tmp/file_";
	child_pid = 0;
	// if (access("./tmp", F_OK))
		// p = "file_";
	file_name = p.append(RandomString(6));
    //file_name = "request_body";

    // std::cout << "fd" << file << std::endl;
    already_checked = false;
	_return = 0;
	readyToParse = false;
	isBodyReady = false;
    firstTime = 0;
    bodyChunked = false;
	file = -1;
	cgiFd = -1;
	uploadedOutFile = -1;
	uploadedInFile = -1;
    ready = false;
	memset(hexBuff, 0, 20);
	hex_len = 0;
	chunk_size = 0;
}

Client::~Client()
{
    
}
