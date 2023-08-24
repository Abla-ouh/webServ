/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelkhei <ebelkhei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:35:35 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/21 11:51:46 by ebelkhei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::Client()
{
    this->locations = _server.getLocation();
    this->status = 0; 
    this->state = BUILDING;
    memset(data, 0, 8000);
}

Client::~Client()
{
    
}