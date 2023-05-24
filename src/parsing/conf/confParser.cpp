#include "confParser.hpp"
#include "confTypes.hpp"
#include "confLexer.hpp"

Parser::Parser(std::string path)
{
	_lxr = new Lexer(path);
	_currentBlock = SERVER;
};


std::pair<values_it, bool>
safe_value_insert(std::pair<values_it, bool> res) {
	std::string err;
	if (!res.second)
		throw Parse_error(err
			+ "duplicate value [ "
			+ *(res.first) + " ] ");
	return res;
}

// THIS IS ATOME O FTHE HOLE THING, EVERYTHING STARTS WITH A DIRCTIVE!!
directive_t Parser::parse_directive(){
	directive_t d;
	Token t = _lxr->get_next_token();
	std::string err;

	err = "unexcpected token: ";
	if (t.token_type == END || t.token_type == SIMICOLONE)
		throw Parse_error(err + t.val);
	d.first = t.val;

	int key_col = t.column;
	t = _lxr->get_next_token();
	while (t.token_type != END && t.token_type != SIMICOLONE) {
		if (key_col != t.column)
			throw Parse_error(err + t.val);
		safe_value_insert(d.second.insert(t.val));
		t = _lxr->get_next_token();
	}
	if (d.second.size() == 0 || t.token_type != SIMICOLONE)
		throw Parse_error(err + t.val);
	return validate(d);
}


std::pair<directives_it, bool>
safe_directive_insert(std::pair<directives_it, bool> res) {
	std::string err;
	if (!res.second)
		throw Parse_error(err
			+ "duplicate directive [ "
			+ res.first->first + " ] ");
	return res;
}


std::pair<locations_it, bool>
safe_location_insert(std::pair<locations_it, bool> res) {
	std::string err;
	if (!res.second)
		throw Parse_error(err 
			+ "duplicate location [ "
			+ res.first->first + " ] ");
	return res;
}

// JUST TO CPMPLICATE THINGS I WILL GO RECURSIVE!
void	Parser::parse_block(Token t) {
	std::string err = "unexcpected token: ";
	
	if (t.token_type == END) return ;

	if (_currentBlock == SERVER) servers.push_back(Server());
	

	// if (_currentBlock == LOCATION)
	// 	servers.back().getLocations().push_back(dirs_t());

	if (t.token_type != SERVER && _currentBlock == SERVER)
			throw Parse_error(err + t.val);
	t = _lxr->get_next_token();
	if (t.token_type == END || t.token_type != LCURL)
		throw Parse_error(err + t.val);
	while (_lxr->peek().token_type != END
		&& _lxr->peek().token_type != RCURL) {
		if (_lxr->peek().token_type == LOCATION
			&& _currentBlock == SERVER) {
			
			// NEED SOME CHECKES!!
			// skip 'location'
			_lxr->get_next_token();
			// get val
			Token t = _lxr->get_next_token();
			_currentLocation = safe_location_insert(
					servers
					.back().getLocations()
					.insert(location_t(t.val, Location()))
			).first;
			_currentBlock = LOCATION;
			parse_block(t);
		}
		else if (_lxr->peek().token_type == DIRCTIVE 
				&& _currentBlock == SERVER)
		{
			safe_directive_insert(
				servers.back().getDirectives()
				.insert(parse_directive())
			);
		}
		else if (_lxr->peek().token_type == DIRCTIVE 
				&& _currentBlock == LOCATION)
		{
			safe_directive_insert(
				_currentLocation->second.getDirectives()
				.insert(parse_directive())
			);		
		}
		else 
			throw Parse_error(err + _lxr->peek().val);
	}
	if (_lxr->peek().token_type != RCURL)
		throw Parse_error(err + _lxr->peek().val);
	_lxr->get_next_token();

	if (_currentBlock == LOCATION) {
		//servers[servers.size() - 1].locations.push_back(l);
		_currentBlock = SERVER;
		return ;
	}
	parse_block(_lxr->get_next_token());	
};


servers_t Parser::operator()(){
	try {
		Token t = _lxr->get_next_token();
		parse_block(t);
		//exit(1);
	}catch (Parse_error &e)
	{
		std::cout << "Error: " << e.what();
		std::cout << " (Line : " << _lxr->_column + 1 << ").\n";
		exit(1);
	}
	return servers;
}


Parser::~Parser(){};