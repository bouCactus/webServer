#include "confParser.hpp"
#include "confTypes.hpp"
#include "confLexer.hpp"

/**
 * @brief   initializing the Parser.
 */
Parser::Parser(std::string path)
{
	_lxr = new Lexer(path);
	_currentBlock = SERVER;
};


/**
 * @brief   Check if the value is inserted in [values].
 * @param	res		pair of iterators to the value being inserted
 *					and  bool indecate whether the insertion
 *				  	is successful or not.
 * @return         	res passed as a parameter.
 */
std::pair<values_it, bool>
safe_value_insert(std::pair<values_it, bool> res) {
	std::string err;
	if (!res.second)
		{ LOG_THROW(); throw Parse_error(err
			+ "duplicate value [ "
			+ *(res.first) + " ] "); }
	return res;
}


/**
 * @brief   		Check if the value is inserted in [directives].
 * @param	res		pair of iterators to the value being inserted
 *					and  bool indecate whether the insertion
 *					is successful or not.
 * @return  	    res passed as a parameter.
 */
std::pair<directives_it, bool>
safe_directive_insert(std::pair<directives_it, bool> res) {
	std::string err;
	if (!res.second)
		{ LOG_THROW(); throw Parse_error(err
			+ "duplicate directive [ "
			+ res.first->first + " ] "); }
	return res;
}

/**
 * @brief   		Check if the value is inserted in [locations].
 * @param	res		pair of iterators to the value being inserted
 *					and  bool indecate whether the insertion
 *					is successful or not.
 * @return       	res passed as a parameter.
 */
std::pair<locations_it, bool>
safe_location_insert(std::pair<locations_it, bool> res) {
	std::string err;
	if (!res.second)
		{ LOG_THROW(); throw Parse_error(err 
			+ "duplicate location [ "
			+ res.first->first + " ] "); }
	return res;
}

/**
 * @brief   		parse a simple directive within a server/location block.
 * @return       	pair of [value_t] represent key and [values_t]
 * 					represent the value.
 */
directive_t Parser::parse_directive(){
	directive_t d;
	Token t = _lxr->getNextToken();
	std::string err;

	err = "unexcpected token: ";
	if (t.token_type == END || t.token_type == SIMICOLONE)
		{ LOG_THROW(); throw Parse_error(err + t.val); }
	d.first = t.val;

	int key_col = t.column;
	t = _lxr->getNextToken();
	while (t.token_type != END && t.token_type != SIMICOLONE) {
		if (key_col != t.column)
			{ LOG_THROW(); throw Parse_error(err + t.val); }
		safe_value_insert(d.second.insert(t.val));
		t = _lxr->getNextToken();
	}
	if (d.second.size() == 0 || t.token_type != SIMICOLONE)
		{ LOG_THROW(); throw Parse_error(err + t.val); }
	return validate(d);
}

/**
 * @brief   		parse a server/location block in a config file.
 * @param	t       The current Token.
 */
void	Parser::parse_block(Token t) {

	std::string err = "unexcpected token: ";

	if (t.token_type == END) return ;
	if (_currentBlock == SERVER) servers.push_back(Server());
	if (t.token_type != SERVER && _currentBlock == SERVER)
			{ LOG_THROW(); throw Parse_error(err + t.val); }
	t = _lxr->getNextToken();
	if (t.token_type == END || t.token_type != LCURL)
		{ LOG_THROW(); throw Parse_error(err + t.val); }
	while (_lxr->peek().token_type != END
		&& _lxr->peek().token_type != RCURL) {
		if (_lxr->peek().token_type == LOCATION
			&& _currentBlock == SERVER) {
			_lxr->getNextToken();
			Token t = _lxr->getNextToken();
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
			{ LOG_THROW(); throw Parse_error(err + _lxr->peek().val);}
	}
	if (_lxr->peek().token_type != RCURL)
		{ LOG_THROW(); throw Parse_error(err + _lxr->peek().val); }
	_lxr->getNextToken();
	if (_currentBlock == LOCATION) {
		_currentBlock = SERVER;
		return ;
	}
	parse_block(_lxr->getNextToken());	
};

/**
 * @brief   		parse the config file.
 * @return	      	servers_t represnt list of servers blocks.
 */
servers_t Parser::operator()(){
	try {
		Token t = _lxr->getNextToken();
		parse_block(t);
	}catch (Parse_error &e)
	{
		std::cout << "Error: " << e.what();
		std::cout << " (Line : " << _lxr->_column + 1 << ").\n";
		exit(1);
	}
	return servers;
}

Parser::~Parser(){};