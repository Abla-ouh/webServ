/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 19:01:08 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/14 19:22:27 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

class Server{
    
    public :
        std::string     serverName; // to replace
        //std::string     port;
        // int             server_socket; // to add 
        // struct addrinfo hint;
        // struct addrinfo *res; // added

        //Server(std::string serv, std::string port); //depricated
        //~Server(); 
        //void    CreateSocket(); // added
};

#endif

