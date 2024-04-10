
#THIS IS NOT A PROPER MAKEFILE#

CPP = c++

#-fsanitize=address -g
CPPFLAGS =  -Wall -Wextra -Werror -std=c++98 -o

SRC = main.cpp Server.cpp Client.cpp Request.cpp request_helpers.cpp ConfigurationFile.cpp

NAME = webserv

all: $(NAME)

$(NAME): $(SRC)
	$(CPP) $(CPPFLAGS) $(NAME) $(SRC)


clean:
	rm $(NAME)

fclean: clean

re: fclean
	make

test:
	$(CPP) -Wall -Werror -Wextra  testmain.cpp Request.cpp request_helpers.cpp -std=c++98 -o test
	@echo --------------------------------------------------------------------------------
	@./test
	@rm test
	@echo --------------------------------------------------------------------------------
run: $(NAME)
	make
	clear
	./$(NAME) config/webserver.conf
