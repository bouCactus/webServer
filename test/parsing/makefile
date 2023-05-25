
MAIN_TEST_PARSING = main
NAME_TEST_PARSING = test_parsing
SRCS_TEST_PARSING =  $(addsuffix .cpp, $(addprefix src/parsing/conf/, $(PARSING)))\
		$(addsuffix .cpp, $(addprefix test/parsing/, $(MAIN_TEST_PARSING)))

OBJ_TEST_PARSING=$(SRCS_TEST_PARSING:.cpp=.o)

test_parsing: $(OBJ_TEST_PARSING)
		@$(CXX)  $(CFLAGS) $(OBJ_TEST_PARSING) $(INC) -o $(NAME_TEST_PARSING)
		@[ -d $(MYDIR) ] || mkdir -p $(MYDIR)
		@[ -d $(BIN) ] || mkdir -p $(BIN)
		@mv $(OBJ_TEST_PARSING) $(MYDIR)
		@mv $(NAME_TEST_PARSING) $(BIN)

