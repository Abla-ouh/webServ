/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouhaga <abouhaga@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/28 12:58:04 by abouhaga          #+#    #+#             */
/*   Updated: 2023/08/29 23:43:44 by abouhaga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPServer.hpp"

size_t getFileSize(const std::string& name)
{
    std::ifstream file(name.c_str(), std::ifstream::ate | std::ifstream::binary);
    size_t size = 0;

    
    if (file.is_open())
    {
        size = file.tellg();
        file.close();
    }
    else {
        file.close();
    }
    return size;
}

char* substr_no_null(const char* str, int start, int length, int str_len) {
    if (start >= str_len)
        return NULL;
    if (start + length > str_len)
        length = str_len - start;
    char* substr = new char[length + 1];
    if (substr == NULL) {
        return NULL;
    }
    const char* src = str + start;
    char* dest = substr;
    while (length-- > 0) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return substr;
}

size_t is_carriage(std::string str, Client &client) {
	
	size_t pos = str.find("\r\n\r\n");
	if (pos == std::string::npos)
		return 0;
	else
    {
		client.readyToParse = true;
		return pos;
	}
}

