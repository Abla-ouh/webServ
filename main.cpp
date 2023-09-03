/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybel-hac <ybel-hac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/03 18:48:07 by abouhaga          #+#    #+#             */
/*   Updated: 2023/09/03 12:06:46 by ybel-hac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/configParse/configFile.hpp"
#include "HTTPServer.hpp"
#include <stdio.h>

void printHeader() {
    std::cout << GREEN  << std::endl;

std::cout <<"   __          __ ______  ____    _____  ______  _____ __      __ ______  _____  " << std::endl;
std::cout <<"   \\ \\        / /|  ____||  _ \\  / ____||  ____||  __ \\ \\    / /|  ____||  __ \\ " << std::endl;
std::cout <<"    \\ \\  /\\  / / | |__   | |_) || (___  | |__   | |__) |\\ \\  / / | |__   | |__) |" << std::endl;
std::cout <<"     \\ \\/  \\/ /  |  __|  |  _ <  \\___ \\ |  __|  |  _  /  \\ \\/ /  |  __|  |  _  / "	<< std::endl;
std::cout <<"      \\  /\\  /   | |____ | |_) | ____) || |____ | | \\ \\   \\  /   | |____ | | \\ \\ "	<< std::endl;
std::cout <<"       \\/  \\/    |______||____/ |_____/ |______||_|  \\_\\   \\/    |______||_|  \\_\\ " << std::endl;

std::cout << "\n            WARNING: This is a student project. Do not use in production.           " << std::endl;
// print all available ports 8080/here

std::cout << WHITE << std::endl;

}


int main(int ac, char const *av[], char **env)
{
	int err_line = 0;
    printHeader();

    if (ac <= 2)
    {
        try {
			string	file = "";
			if (av[1])
				file += av[1];
            std::cout << GREEN <<"\nAvailable ports: " << std::endl;
            configFile config(file, env, &err_line);
            HTTPServer server(config);
            server.start();
        }
        catch (std::exception& err)
        {
            std::cerr << RED "Error in line " << err_line << " : " << err.what() << WHITE << std::endl;
        }
    }
    else
    {
        std::cout << RED "Usage:" << WHITE " ./webserv <config file> \"" << YELLOW "./webserv message.conf\"" << WHITE << std::endl;
    }
    return 0;
}
