/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:35:35 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/17 19:27:59 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::Client()
{
}


Client::Client(int socket, const std::string& httpRequest): request(httpRequest)
{
    //server_id = 0;
    this->status = 0; 
    memset(data, 0, 8000);
    client_socket = socket;
}

Client::~Client()
{
    
}