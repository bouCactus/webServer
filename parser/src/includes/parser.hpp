#pragma once

#include <iostream>
#include "./lexer.hpp"
#include "./parse_error.hpp"
#include <vector>
#include <map>


struct directive {
	std::string					key;
	std::vector<std::string>	values;
};

struct location {
	std::vector<directive> directives;
};

struct server {
	std::vector<directive> directives;
	std::vector<location> locations;
};

class Parser {
	private:
		Lexer *_lxr;
		Parser(Parser &){};
		Parser &operator=(Parser &p){return p;};
		std::vector<server> servers;
	public:
		Parser(std::string path);
		std::vector<server> parse();
		server parse_server();
		location parse_location();
		directive	parse_directive();
		void		print_servers();
		~Parser();
};
