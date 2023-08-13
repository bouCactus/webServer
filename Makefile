NAME = webserver
INC = $(addprefix -I ,$(wildcard include/*))
RM = rm -rf
BUILD = build
MYDIR = $(BUILD)/objs 
BIN = $(BUILD)/bin
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror #-fsanitize=address
OBJ = $(SRCS:.cpp=.o)
OBJBONUS = $(SRCBONUS:.cpp=.o)

SESSION = HttpClient HttpServer fileSystem HttpResponse HttpRequest HttpMethodProcessor \
		  utilsFunction HttpMethodHelpers CGI
PARSING = confAST confLexer confParser confValidate
MAIN = main

SRCS = $(addsuffix .cpp, $(addprefix src/session/, $(SESSION))) \
		$(addsuffix .cpp, $(addprefix src/parsing/conf/, $(PARSING)))

# Define the total number of files to be processed
TOTAL_FILES := $(words $(SRCS))

# Define the progress bar width
BAR_WIDTH := 50

# Initialize the CURRENTFILE variable to 0
CURRENTFILE := 0

# ANSI escape codes for color formatting
COLOR_GREEN = \033[1;32m
COLOR_RESET = \033[0m

define PRINT_PROGRESS
	@printf "\r[%-${BAR_WIDTH}s] %3d%% %d/%d ${COLOR_GREEN}%s${COLOR_RESET}" \
		"$(shell printf '=%.0s' $$(seq 1 $$(( $(CURRENTFILE) * $(BAR_WIDTH) / $(TOTAL_FILES) ))))" \
		"$$(($(CURRENTFILE) * 100 / $(TOTAL_FILES)))" "$(CURRENTFILE)" "$(TOTAL_FILES)" "$4"
	@printf "\r"
endef

# Extract the file name from the full path (e.g., "src/session/HttpClient.cpp" -> "HttpClient.cpp")
get_filename = $(notdir $1)

.cpp.o:
	@$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@
	@$(eval CURRENTFILE=$(shell echo $$(($(CURRENTFILE)+1))))
	@$(call PRINT_PROGRESS,$(CURRENTFILE),$(TOTAL_FILES),$(BAR_WIDTH),$(call get_filename,$<))

all: $(NAME)
	@echo "${COLOR_GREEN}*************** DONE *************************${COLOR_RESET}"
	@echo "run ${COLOR_GREEN}./${BIN}/${NAME}${COLOR_RESET} to execute program"
	@echo "${COLOR_GREEN}**********************************************${COLOR_RESET}"
	

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) -fsanitize=address src/main.cpp $(OBJ) $(INC) -o $(NAME)
	@echo  
	@[ -d $(MYDIR) ] || mkdir -p $(MYDIR)
	@[ -d $(BIN) ] || mkdir -p $(BIN)
#	@mv $(OBJ) $(MYDIR)
#	@mv $(NAME) $(BIN)

clean:
	@$(RM) $(OBJ) $(MYDIR)
	@echo "${COLOR_GREEN}************* Removed **************${COLOR_RESET}"

fclean: clean
	@$(RM) $(NAME) $(BUILD)

re: fclean all

run: 
	$(BIN)/$(NAME)  ./test/multiplexer/file.conf

# include test/parsing/Makefile.mk
# include test/simpleResponse/Makefile.mk
# include test/multiplexer/Makefile.mk

.PHONY: all clean fclean re run
