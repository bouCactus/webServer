#pragma once
#include <iostream>
#include "./confTypes.hpp"


struct Token {
	
	int			start;
	int			column;
	std::string val;
	Type		token_type;
	Token(Type type, std::string v,int s = 0, int col = 0)
		: start(s), column(col),val(v), token_type(type){};
};

class Lexer {
	public:
		std::string _source;
		std::string::size_type			_current;
		std::string::size_type			_column;
		Lexer(Lexer &){};
		Lexer &operator=(Lexer &l){return l;};
	public:
		Lexer(std::string path);
		void	skipComment();
		void	skipEmptyLines();
		void	log(int print_source);
		Token	getNextToken();
		Token	peek();
		~Lexer();
};
