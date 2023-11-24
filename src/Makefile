
#THIS IS NOT A PROPER MAKEFILE#

CPP = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -o

SRC = main.cpp Server.cpp Client.cpp 

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
	$(CPP) testmain.cpp Client.cpp Request.cpp -std=c++98 -o test
	@echo --------------------------------------------------------------------------------
	@./test
	@rm test
	@echo --------------------------------------------------------------------------------
