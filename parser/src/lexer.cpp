#include "includes/lexer.hpp"
#include <fstream>
#include "includes/parse_error.hpp"
#include <sstream>
#include <cctype>

Lexer::Lexer(std::string path) : _current(0), _column(0)
{
	// IF YOU DON'T MIND, I AM JUST GOING READ YOUR FILE
	std::ifstream file;
	file.open(path);
	// JSUT INCASE YOU WANT TO PLAY WITH US, I WILL THROW IT INTO YOUR FACE!
	if (!file.is_open())
		throw Parse_error("config file is invalid.");
	// NO BODY REALLY UNDERSTAND THE STREAM STUFF, BUT IT IS JUST TO TURNE YOUR FILE INTO STRING.
	std::stringstream buffer;
	buffer << file.rdbuf();
	this->_source = buffer.str();
};


void	Lexer::skip_space(){
	// JUST READ THE NAME
	while (_current != _source.length()
			&& isspace(_source[_current]))
		_current++;
};

void	Lexer::skip_comment(){
	// YES EXACTLY, I WILL NOT SEE YOUR COMMENTS
	while (_current != _source.length()
			&& _source[_current] != '\n')
		_current++;
};

void	Lexer::skip_nl_and_space(){
		// AGAIN READ THE NAME
		while (_current != _source.length()
			&& (_source[_current] == '\n' || isspace(_source[_current])))
		{
			if (_source[_current] == '\n')
				_column++;
			_current++;
		}
};


// YOU REALLY DON'T HAVE TO READ THIS, IT IS JUST FOR ME, YES EXACTLY!!
void	Lexer::log(int print_source){
	if (print_source)
	{
		std::cout << "Source: " << std::endl;
		std::cout << this->_source << std::endl;
	}
	std::cout <<"\n-----------------------------------------------------\n";
	std::cout << "| Current Char : ";
	std::cout << this->_source[this->_current];
	std::cout << "\tAt: line: " << this->_column;
	std::cout << "\tposition: " << this->_current;
	std::cout << " |" << std::endl;
	std::cout <<"-----------------------------------------------------\n";
};

// HAVE EVER SOMEONE TOLD YOU THAT YOU ARE SO SPECIAL!!
int is_special(char c)
{
	return (c == ';' || c == '{'
		|| c == '}' || c == '#'
		|| c == '\n');
}

// NEXT AND NEXT AND YOU WILL GET THERE.
Token	Lexer::get_next_token(){

	int start = 0;
	while (_current != _source.length())
	{
		// LET'S JUST IGONE THE HOLE EMPTY THINGS
		skip_nl_and_space();
		// THERE IS A CASE AND THERE IS ALWASE SOMEONE WHO WILL SOLVE IT AT THE END.
		if (_current != _source.length())
		{
			switch(_source[_current])
			{
				case '#':
					skip_comment();
					break;
				case ';':
					++_current;
					return Token(SIMICOLONE, ";",
						_current - 1, _column);
					break;
				case '{' :
					++_current;
					return Token(LCURL, "{",
						_current - 1, _column);
					break;
				case '}':
					++_current;
					return Token(RCURL, "}" ,
							_current - 1, _column);
				case '\n':
					break;
				default:
				{
					start = _current;
					while (_current != _source.length()
						&& !isspace(_source[_current])
						&& !is_special(_source[_current]))
					{
						_current++;
					}
					std::string val = _source.substr(start, _current - start);
					//std::cout << val << " token is at: " << _column << "\n"; 
					if (val == "server")
						return (Token(SERVER, val ,start, _column));
					else if (val == "location")
						return (Token(LOCATION, val, start, _column));
					return (Token(DIRCTIVE, val , start, _column));
				}
			}
		}
	}
	//std::cout << "token is at: " << _column << "\n"; 
	return (Token(END, ""));
};

// THIS IS THE THE EYE THAT PREDECT THE FUTURE OF YOU TOKENS
Token	Lexer::peek(){
	int s = _current;
	int c = _column;
	Token t = get_next_token();
	_current = s;
	_column = c;
	return (t);
}

Lexer::~Lexer(){};

