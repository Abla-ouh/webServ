/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelkhei <ebelkhei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:35:35 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/18 16:47:56 by ebelkhei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::Client()
{
    this->locations = _server.getLocation();
    this->status = 0; 
    memset(data, 0, 8000);
}

Client::~Client()
{
    
}