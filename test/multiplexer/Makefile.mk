
MAIN= test/multiplexer/main.cpp
NAME=multiplexer

SRC_SERVER= ${MAIN}
OBJ_SERVER=$(SRC_SERVER:.cpp=.o)

multiplexer: $(OBJ_SERVER)
	@	$(CXX)  $(CFLAGS) $(MAIN) -o $(NAME)
	@	[ -d $(MYDIR) ] || mkdir -p $(MYDIR)
	@	[ -d $(BIN) ] || mkdir -p $(BIN)
	@	mv $(OBJ_SERVER) $(MYDIR)
	@	mv $(NAME) $(BIN)
