/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/03 18:48:07 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/22 21:39:40 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/configParse/configFile.hpp"
#include "utils/configParse/configFile.hpp"
#include "HTTPServer.hpp"
#include<stdio.h>

// void printBody(const std::string &filename) {
//     std::ifstream bodyFile(filename.c_str(), std::ios::in | std::ios::binary);
//     if (!bodyFile.is_open()) {
//         std::cerr << "Error opening request body file" << std::endl;
//         return;
//     }

//     char c;
//     while (bodyFile.get(c)) {
//         std::cout << c;
//     }

//     bodyFile.close();
// }

int main(int ac, char const *av[])
{
	if (ac == 2)
	{
		try {
			configFile config((string(av[1])));
            HTTPServer server(config);
            server.start();
			// config.print();
    		//printBody(filename);
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


