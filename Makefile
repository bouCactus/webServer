NAME = webserver
INC = -I include
RM = rm -rf
MYDIR= objects_files
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror
OBJ=$(SRCS:.cpp=.o)
OBJBONUS=$(SRCBONUS:.cpp=.o)

SESSION = HttpResponse HttpRequest HttpMethodProcessor HttpClient
MAIN = main

SRCS =  $(addsuffix .cpp, $(addprefix src/session/, $(SESSION))) \
	$(addsuffix .cpp, $(addprefix src/, $(MAIN)))

.cpp.o:
	@$(CXX)  $(CXXFLAGS) $(INC) -c $< -o $@

all : $(NAME)

$(NAME): $(OBJ)
	@$(CXX)  $(CFLAGS) $(OBJ) $(INC)  -o $(NAME)
	@[ -d $(MYDIR) ] || mkdir -p $(MYDIR)
	@mv $(OBJ) objects_files

clean :
	@$(RM) $(OBJ) $(MYDIR)

fclean : clean
	@$(RM) $(NAME) 

re : fclean all

.PHONY:			all clean fclean re bonus
