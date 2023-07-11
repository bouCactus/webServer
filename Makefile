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

SESSION =  HttpClient HttpServer fileSystem HttpResponse HttpRequest HttpMethodProcessor
PARSING = confAST confLexer confParser confValidate
MAIN = main

SRCS =  $(addsuffix .cpp, $(addprefix src/session/, $(SESSION))) \
		$(addsuffix .cpp, $(addprefix src/parsing/conf/, $(PARSING)))\


.cpp.o:
	@$(CXX)  $(CXXFLAGS) $(INC) -c $< -o $@

all : $(NAME)
	@echo "\033[1;32m*************** DONE *************************\033[0m"
	@echo "run \033[1;32m./${BIN}/${NAME}\033[0m to execute program"
	@echo "\033[1;32m**********************************************\033[0m"

$(NAME): $(OBJ)
	@$(CXX)  $(CFLAGS) src/main.cpp $(OBJ) $(INC) -o $(NAME)
	@echo .
	@[ -d $(MYDIR) ] || mkdir -p $(MYDIR)
	@[ -d $(BIN) ] || mkdir -p $(BIN)
	@mv $(OBJ) $(MYDIR)
	@mv $(NAME) $(BIN)

clean :
	@$(RM) $(OBJ) $(MYDIR)
	@echo "\033[1;31m************* Removed **************\033[0m"

fclean : clean
	@$(RM) $(NAME) $(BUILD)

re : fclean all

run : all clean
	$(BIN)/$(NAME)

include test/parsing/Makefile.mk
include test/simpleResponse/Makefile.mk
include test/multiplexer/Makefile.mk


.PHONY:			all clean fclean re bonus test_parsing