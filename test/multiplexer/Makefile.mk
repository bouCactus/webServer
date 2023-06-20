
MAIN= test/multiplexer/main.cpp
NAME_MULTIPLEXER=multiplexer

SRC_SERVER= ${MAIN}
OBJ_SERVER=$(SRC_SERVER:.cpp=.o)

multiplexer: $(OBJ_SERVER)
	@	$(CXX)  $(CFLAGS) $(MAIN) -o $(NAME_MULTIPLEXER)
	@	[ -d $(MYDIR) ] || mkdir -p $(MYDIR)
	@	[ -d $(BIN) ] || mkdir -p $(BIN)
	@	mv $(OBJ_SERVER) $(MYDIR)
	@	mv $(NAME_MULTIPLEXER) $(BIN)
