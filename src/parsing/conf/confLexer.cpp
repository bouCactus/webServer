#include "confLexer.hpp"
#include "confTypes.hpp"
#include "confParseError.hpp"
#include <fstream>
#include <sstream>
#include <cctype>

/**
 * @brief   Initilazing the Lexer.
 * @param   path	the path to the config file.
 */
Lexer::Lexer(std::string path) : _current(0), _column(0)
{
	std::ifstream file;
	file.open(path);
	if (!file.is_open())
		{ LOG_THROW(); throw Parse_error("config file is invalid."); }
	std::stringstream buffer;
	buffer << file.rdbuf();
	this->_source = buffer.str();
};

/**
 * @brief   Skip a line.
 */
void	Lexer::skipComment(){
	while (_current != _source.length()
			&& _source[_current] != '\n')
		_current++;
};

/**
 * @brief   Skip a empty lines.
 */
void	Lexer::skipEmptyLines(){
		while (_current != _source.length()
			&& (_source[_current] == '\n'
			|| isspace(_source[_current])))
		{
			if (_source[_current] == '\n')
				_column++;
			_current++;
		}
};


/**
 * @brief   Log the current char proccessed by the lexer,
 * 			with its position in the file.
 */
void	Lexer::log(int print_source){
	if (print_source)
	{
		std::cout << "Source: " << std::endl;
		std::cout << this->_source << std::endl;
	}
	std::cout <<"\n----------------------------------------\n";
	std::cout << "| Current Char : ";
	std::cout << this->_source[this->_current];
	std::cout << "\tAt: line: " << this->_column;
	std::cout << "\tposition: " << this->_current;
	std::cout << " |" << std::endl;
	std::cout <<"----------------------------------------\n";
};

/**
 * @brief   Check if a char is special value.
 */
int is_special(char c)
{
	return (c == ';' || c == '{'
		|| c == '}' || c == '#'
		|| c == '\n');
}

/**
 * @brief   Point to the next token and return it.
 * @return	the next Token.
 */
Token	Lexer::getNextToken(){

	int 		start = 0;
	std::string val;
	Type		type;

	while (_current != _source.length())
	{
		skipEmptyLines();
		if (_current == _source.length()) break;		
		switch(_source[_current])
		{
			case '#': skipComment(); break;
			case ';':
				return Token(SIMICOLONE, ";",
						_current++, _column);
			case '{' :
				return Token(LCURL, "{",
						_current++, _column);
			case '}':
				return Token(RCURL, "}" ,
						_current++, _column);
			case '\n': break;
			default:
			{
				start = _current;
				while (_current != _source.length()
					&& !isspace(_source[_current])
					&& !is_special(_source[_current]))
					_current++;
				val = _source.substr(start, _current - start);
				if (val == "server") type = SERVER;
				else if (val == "location") type = LOCATION;
				else type = DIRCTIVE;
				return (Token(type, val , start, _column));
			}
		}
		
	}
	return (Token(END, ""));
};

/**
 * @brief   return next token without pointing to it.
 * @return	the next Token.
 */
Token	Lexer::peek(){
	int s = _current;
	int c = _column;
	Token t = getNextToken();
	_current = s;
	_column = c;
	return (t);
}

Lexer::~Lexer(){};

