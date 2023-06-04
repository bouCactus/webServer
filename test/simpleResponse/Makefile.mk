
NAME_TEST_RESPONSE = test_respond
MAIN= test/simpleResponse/main.cpp
SERVER_NAME=server


$(NAME_TEST_RESPONSE): $(OBJ)
		@$(CXX)  $(CFLAGS) $(MAIN) $(OBJ) $(INC) -o $(SERVER_NAME)
		@[ -d $(MYDIR) ] || mkdir -p $(MYDIR)
		@[ -d $(BIN) ] || mkdir -p $(BIN)
		@mv $(OBJ) $(MYDIR)
		@mv $(SERVER_NAME) $(BIN)



