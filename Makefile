NAME = webserver
INC = $(addprefix -I ,$(wildcard include/*))
RM = rm -rf
BUILD= build
MYDIR= $(BUILD)/objs 
BIN= $(BUILD)/bin
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror
OBJ=$(SRCS:.cpp=.o)
OBJBONUS=$(SRCBONUS:.cpp=.o)

SESSION = HttpResponse HttpRequest HttpMethodProcessor HttpClient
PARSING = confAST confLexer confParser confValidate
MAIN = main

SRCS =  $(addsuffix .cpp, $(addprefix src/session/, $(SESSION))) \
		$(addsuffix .cpp, $(addprefix src/parsing/conf/, $(PARSING)))\
		$(addsuffix .cpp, $(addprefix src/, $(MAIN)))

.cpp.o:
	$(CXX)  $(CXXFLAGS) $(INC) -c $< -o $@

all : $(NAME)

$(NAME): $(OBJ)
	@$(CXX)  $(CFLAGS) $(OBJ) $(INC) -o $(NAME)
	@[ -d $(MYDIR) ] || mkdir -p $(MYDIR)
	@[ -d $(BIN) ] || mkdir -p $(BIN)
	@mv $(OBJ) $(MYDIR)
	@mv $(NAME) $(BIN)

clean :
	@$(RM) $(OBJ) $(MYDIR)

fclean : clean
	@$(RM) $(NAME) $(BUILD)

re : fclean all

include test/parsing/makefile

.PHONY:			all clean fclean re bonus test_parsing
