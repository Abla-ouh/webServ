/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 18:32:03 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/22 19:16:22 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

//         Client();
//         Client(int socket, const std::string& httpRequest);
//         ~Client();

//         char                    data[8000];
//         Request&                getRequest() { return request;};
//         Response&               getResponse() { return response;};
//         int                     getStatus() { return status;};
//         location&               getlocation() { return _location; };
//         std::vector<location>&  getlocations() { return locations; };
//         int                     getClientSocket() { return client_socket;};
//         server                  getServer() { return _server;};

//         void    setRequest(Request other) { this->request = other; };
//         void    setStatus(int other) { this->status = other; };
//         void    setlocation(location other) { this->_location = other;};
//         void    setlocations(std::vector<location> other) { this->locations = other; };
//         void    setClientSocket(int other) { this->client_socket = other;};
//         void    setServer(server other) { this->_server = other;};
// };


#endif