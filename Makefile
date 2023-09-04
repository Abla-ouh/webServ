CONFIG_PARSE = ./utils/configParse/
RESPONSE = ./Response/

SRCS = main.cpp Client.cpp HTTPServer.cpp Request.cpp RequestUtils.cpp\
       $(CONFIG_PARSE)configFile.cpp\
       $(CONFIG_PARSE)serverClass.cpp\
	   $(CONFIG_PARSE)locationClass.cpp\
       $(CONFIG_PARSE)configFile_utils.cpp\
       $(CONFIG_PARSE)../postMethode/Post.cpp\
       $(CONFIG_PARSE)../autoindex/createAutoindexPage.cpp\
       $(RESPONSE)locationMatching.cpp\
       $(RESPONSE)response.cpp\
       $(RESPONSE)utils.cpp\
	   $(CONFIG_PARSE)../CGI/cgi.cpp\
	   ./delete/delete.cpp
NAME = webserv
CC = g++
CXX = g++

# Color codes
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
RESET = \033[0m


# Formatting
BOLD := \033[1m
UNDERLINE := \033[4m

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(YELLOW)$(BOLD)Linking...$(RESET)"
	$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(BOLD)$(NAME) has been built successfully!$(RESET)"

clean:
	@echo "$(RED)$(BOLD)Cleaning object files...$(RESET)"
	rm -rf $(OBJS)

fclean: clean
	@echo "$(RED)$(BOLD)Cleaning $(NAME) executable...$(RESET)"
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean clean_files re
