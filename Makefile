NAME = webserv
CPP = c++
FLAGS =  -Wall -Wextra -Werror -std=c++11 -g -fsanitize=address
SRCDIR = src
OBJDIR = obj
SRC = 	src/main.cpp \
		src/Server.cpp \
		src/Client.cpp \
		src/Request.cpp \
		src/request_helpers.cpp \
		src/ConfigurationFile.cpp \
		src/Location.cpp \
		src/Host.cpp \
		src/Response.cpp
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

.PHONY: all clean fclean re run test

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(NAME): $(OBJ)
	$(CPP) $(FLAGS) -o $(NAME) $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(NAME)

fclean: clean

re: fclean
	make

run: $(NAME)
	make
	clear
	./$(NAME) config/webserver.conf

test:
	$(CPP) -Wall -Werror -Wextra  testmain.cpp Request.cpp request_helpers.cpp -o test
	@echo --------------------------------------------------------------------------------
	@./test
	@rm test
	@echo --------------------------------------------------------------------------------
	
