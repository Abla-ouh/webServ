#SRCS		= multiClients/main.cpp multiClients/Client.cpp multiClients/HTTPServer.cpp multiClients/Server.cpp
CONFIG_PARSE = ./utils/configParse/
RESPONSE = ./Response/

SRCS = main.cpp Client.cpp HTTPServer.cpp Request.cpp\
		$(CONFIG_PARSE)configFile.cpp\
		$(CONFIG_PARSE)serverClass.cpp\
		$(CONFIG_PARSE)configFile_utils.cpp\
		$(CONFIG_PARSE)../postMethode/Post.cpp\
		$(CONFIG_PARSE)../autoindex/createAutoindexPage.cpp\
		$(RESPONSE)locationMatching.cpp\
		$(RESPONSE)response.cpp\
		$(RESPONSE)utils.cpp

NAME		= webserv
CC	= g++
CXX = g++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

OBJS		= $(SRCS:.cpp=.o)

all:		$(NAME)

$(NAME)	: 
		@$(CC) $(CFLAGS) $(SRCS) -o $(NAME)

clean:
		@rm -rf $(OBJS)

fclean:	clean
		@rm -rf $(NAME)

re:	fclean all

up: all clean
	@echo "\033[0;35m 🚀 $(NAME) was successfully created \033[0m"
