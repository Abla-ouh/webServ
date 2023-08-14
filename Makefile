NAME		= wwebserv
#SRCS		= multiClients/main.cpp multiClients/Client.cpp multiClients/HTTPServer.cpp multiClients/Server.cpp
SRCS		= main.cpp Client.cpp HTTPServer.cpp Server.cpp Request.cpp

CC	= c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

OBJS		= $(SRCS:.cpp=.o)

all:		$(NAME)

$(NAME)	: $(OBJS)
		$(CC) $(CFLAGS) $(SRCS) -o $(NAME)

clean:
		rm -rf $(OBJS)

fclean:	clean
		rm -rf $(NAME)

re:	fclean all
