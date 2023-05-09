#pragma once
#include <iostream>


enum Type {
	SERVER,
	DIRCTIVE,
	LOCATION,
	SIMICOLONE,
	LCURL,
	RCURL,
	END
};

struct Token {
	
	int			start;
	int			column;
	std::string val;
	Type		token_type;
	Token(Type type, std::string v,int s = 0, int col = 0)
		: token_type(type), val(v),start(s), column(col){};
};

class Lexer {
	public:
		std::string _source;
		int			_current;
		int			_column;
		Lexer(Lexer &){};
		Lexer &operator=(Lexer &l){return l;};
	public:
		Lexer(std::string path);
		void	skip_space();
		void	skip_comment();
		void	skip_nl_and_space();
		void	log(int print_source);
		Token	get_next_token();
		Token	peek();
		~Lexer();
};
