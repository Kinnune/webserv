
#THIS IS NOT A PROPER MAKEFILE#

CPP = c++

#-fsanitize=address -g
CPPFLAGS =  -Wall -Wextra -Werror -o

SRC = src/main.cpp src/Server.cpp src/Client.cpp src/Request.cpp src/request_helpers.cpp src/ConfigurationFile.cpp

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
	$(CPP) -Wall -Werror -Wextra  testmain.cpp Request.cpp request_helpers.cpp -o test
	@echo --------------------------------------------------------------------------------
	@./test
	@rm test
	@echo --------------------------------------------------------------------------------
run: $(NAME)
	make
	clear
	./$(NAME) config/webserver.conf
