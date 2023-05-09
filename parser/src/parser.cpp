#include "./includes/parser.hpp"
#include "./includes/parse_error.hpp"

Parser::Parser(std::string path)
{
	_lxr = new Lexer(path);
};

// THIS IS ATOME O FTHE HOLE THING, EVERYTHING STARTS WITH A DIRCTIVE!!
directive Parser::parse_directive(){
	directive d;
	Token t = _lxr->get_next_token();

	if (t.token_type == END || t.token_type == SIMICOLONE)
		throw Parse_error(std::string("") + "unexcpected token: " + t.val);
	d.key = t.val;
	int key_col = t.column;
	t = _lxr->get_next_token();
	//std::cout << "key column = " << t.column << "\n";
	while (t.token_type != END && t.token_type != SIMICOLONE)
	{
		//excpecting simicolon here!!
		//std::cout << "t : " << t.val<< " val : " << key_col << " col : " << t.column << "\n";
		if (key_col != t.column)
			throw Parse_error(std::string("") + "unexcpected token: " + t.val);
		d.values.push_back(t.val);
		t = _lxr->get_next_token();
	}
	

	if (d.values.size() == 0 || t.token_type != SIMICOLONE)
		throw Parse_error(std::string("") + "unexcpected token: " + t.val);
	return d;
}

// LOCTION I SJUST A BUNCH OF DIRCTIVE RIGHT? I'LL HANDLE THEM
location Parser::parse_location(){
	location l;
	_lxr->get_next_token();
	Token t = _lxr->get_next_token();
	
	if (t.token_type == END || t.token_type != LCURL)
		throw Parse_error(std::string("") + "unexcpected token: " + t.val);
	//t = _lxr->get_next_token();
	while (_lxr->peek().token_type != END && _lxr->peek().token_type != RCURL)
	{
		if (_lxr->peek().token_type == DIRCTIVE)
			l.directives.push_back(parse_directive());
		else
			throw Parse_error(std::string("") + "unexcpected token: " + _lxr->peek().val);
		//t = _lxr->get_next_token();
	}
	if (_lxr->peek().token_type != RCURL)
		throw Parse_error(std::string("") + "unexcpected token: " + _lxr->peek().val);
	_lxr->get_next_token();	
	return l;
}

// I AM LOCKING FOR A LOCTION? NOTHING?? HOW A ABOUT A DIRCTIVE?
server	Parser::parse_server(){

	server s;
	Token t = _lxr->get_next_token();
	if (t.token_type == END || t.token_type != LCURL)
		throw Parse_error(std::string("") + "unexcpected token: " + t.val);
	
	//t = _lxr->get_next_token();
	while (_lxr->peek().token_type != END && _lxr->peek().token_type != RCURL)
	{
		
		//std::cout << "in " << _lxr->peek().val << "\n";	
		if (_lxr->peek().token_type == LOCATION)
			s.locations.push_back(parse_location());
		else if (_lxr->peek().token_type == DIRCTIVE)
			s.directives.push_back(parse_directive());
		else 
			throw Parse_error(std::string("") + "unexcpected token: " + _lxr->peek().val);
		//t = _lxr->get_next_token();
	}
	if (_lxr->peek().token_type != RCURL)
		throw Parse_error(std::string("") + "unexcpected token: " + _lxr->peek().val);
	_lxr->get_next_token();		
	return s;
};

// WATCH, I WILL PARSE EACH SERVER ALONE
std::vector<server> Parser::parse()
{

	Token t = _lxr->get_next_token();
	while (t.token_type != END)
	{
		if (t.token_type != SERVER)
			throw Parse_error(std::string("") + "unexcpected token: " + t.val);
			servers.push_back(parse_server());
		t = _lxr->get_next_token();
	}
	return servers;
}



// YOU SHOULD NOT READ THIS CODE! 
void	Parser::print_servers()
{
	for(int i = 0; i < this->servers.size(); i++)
	{
		std::cout << "----- server " << i+1 << "------\n";
		std::cout << "\t----directives----\n";
		for(int j = 0; j < this->servers[i].directives.size(); j++)
		{
			std::cout << "\t key : " << this->servers[i].directives[j].key << " \t\t\t| ";
			std::cout << " values: ";
			for (int e = 0; e < this->servers[i].directives[j].values.size(); e++)
			{
				std::cout << this->servers[i].directives[j].values[e] << " ";
			}
			std::cout << "\n";
		} 
		std::cout << "-------- loactions ---------\n";
		for(int c = 0; c < this->servers[i].locations.size(); c++)
		{
				std::cout << "\t loaction " << c << "\n";
				for(int f = 0; f < this->servers[i].locations[c].directives.size(); f++)
				{
					std::cout << "\t\t key : " << this->servers[i].locations[c].directives[f].key << " \t\t\t| ";
					std::cout << " values: ";
					for (int e = 0; e < this->servers[i].locations[c].directives[f].values.size(); e++)
					{
						std::cout << this->servers[i].locations[c].directives[f].values[e] << " ";
					}
					std::cout << "\n";
				} 
		}
	}
}
// I TOLD YOU.

Parser::~Parser(){};