#include "confTypes.hpp"
#include "confParser.hpp"
#include "confParseError.hpp"

Key	getKey(std::string key)
{
	if (key == "client_max_body_size") return MAX;
	if (key == "listen") return LISTEN;
	if (key == "server_name") return SERVER_NAME;
	if (key == "index") return INDEX;
	if (key == "error_page") return ERROR_PAGE;
	if (key == "path") return PATH;
	if (key == "root") return ROOT;
	if (key == "autoindex") return AUTOINDEX;
	if (key == "set") return SET;
	if (key == "cgi_pass") return CGI;
	if (key == "host") return HOST;
	if (key == "allow") return ALLOW;
	if (key == "return") return RETURN;
	if (key == "cgi_allow") return CGI_ALLOW;
	throw Parse_error(std::string("") + "unkown directive " + key);
}

int str_is_num(std::string s)
{
	for(size_s i = 0; i < s.size(); i++)
		if (!isdigit(s[i]))	return 0;
	return 1;
}



void Parser::validate_listen(values_t values){
	std::string err;

	if (_currentBlock == LOCATION)
		throw Parse_error(err + "unexpected listen directive in location block");
	values_it it = values.begin();
	for (; it != values.end(); it++)
	{
		if (str_is_num((*it).substr(0, (*it).length())))
			continue ;
		throw Parse_error(std::string("") + "port should a number.");
	}
	return ;
};

void Parser::validate_host(values_t values){
	std::string err;
	if (values.size() != 1)
		throw Parse_error(err + "too many values in host directive");
	return ;
}

void Parser::validate_error_page(values_t	values){(void)values;};

void Parser::validate_server_name(values_t	values){(void)values;};

void Parser::validate_max(values_t values){
	if (values.size() != 1) 
		throw Parse_error(std::string("") + "client_max_body_size should have only one value");
	value_t val = *(values.begin());
	if (val.length() > 1 && val[val.length() - 1] == 'M' 
		&& str_is_num(val.substr(0, val.length() - 1)))
		return ;
	throw Parse_error(std::string("") + "client_max_body_size should be a number followed by M.");
};

void Parser::validate_index(values_t		values){(void)values;};
void Parser::validate_path(values_t			values){(void)values;};
void Parser::validate_root(values_t			values){(void)values;};
void Parser::validate_autoindex(values_t	values){
	std::string err;
	if (values.size() != 1)
		throw Parse_error(err + "too many values in autoindex directive");
	value_t val = *(values.begin());
	if (val != "on" && val != "off")
		throw  Parse_error(err + "unkonwn vaue [ " + val + " ] in autoindex directive");
};
void Parser::validate_set(values_t			values){(void)values;};
void Parser::validate_cgi(values_t			values){(void)values;};	
void Parser::validate_allow(values_t		values){(void)values;};
void Parser::validate_return(values_t		values){(void)values;};
void Parser::validate_cgi_allow(values_t	values){(void)values;};

// OH, YOU JUST CAN'T WRITE ANY GARBAGE TEXT
directive_t	Parser::validate(directive_t d)
{
	Key key = getKey(d.first);
	switch (key)
	{
		case MAX : 			validate_max(d.second); 		break;
		case LISTEN: 		validate_listen(d.second); 		break;
		case HOST:			validate_host(d.second);		break;
		case INDEX: 		validate_index(d.second); 		break;
		case ERROR_PAGE: 	validate_error_page(d.second); 	break;
		case PATH: 			validate_path(d.second); 		break;
		case ROOT: 			validate_root(d.second); 		break;
		case AUTOINDEX: 	validate_autoindex(d.second); 	break;
		case SET: 			validate_set(d.second);			break;
		case CGI: 			validate_cgi(d.second);			break;
		case ALLOW:			validate_allow(d.second);		break;
		case RETURN:		validate_return(d.second);		break;
		case CGI_ALLOW:		validate_cgi_allow(d.second);	break;

		default: break;
	}
	
	return d;
}