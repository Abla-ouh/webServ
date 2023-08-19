/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/03 18:48:07 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/19 14:35:46 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/configParse/configFile.hpp"
#include "HTTPServer.hpp"
#include<stdio.h>


int main(int ac, char const *av[])
{
	if (ac == 2)
	{
		try {
			configFile config((string(av[1])));
            HTTPServer server(config);
			// config.print();
            server.start();
		}
		catch (std::exception& err)
		{
			std::cerr << RED "Error: " << err.what();
		}
	}
	else
		cout << RED "using: ./webserv <config file> \"./webserve message.conf\"\n";
	return 0;
}

