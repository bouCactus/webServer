#include "confTypes.hpp"
#include "confParser.hpp"
#include "confParseError.hpp"
#include <sstream>

Key	getKey(std::string key)
{
	if (key == "client_max_body_size") return MAX;
	if (key == "listen") return LISTEN;
	if (key == "server_name") return SERVER_NAME;
	if (key == "index") return INDEX;
	if (key == "error_page") return ERROR_PAGE;
	if (key == "root") return ROOT;
	if (key == "autoindex") return AUTOINDEX;
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
	return !s.size() ? 0 : 1;
}
int str_isalnum(std::string s) {
	for(size_s i = 0; i < s.size(); i++)
		if (!isalnum(s[i]))	return 0;
	return !s.size() ? 0 : 1;
}



void Parser::validate_listen(values_t values){
	std::string err;

	if (_currentBlock == LOCATION)
		throw Parse_error(err + "unexpected listen directive in location block");
	values_it it = values.begin();
	for (; it != values.end(); it++)
	{
		char *str;
		std::string port = (*it).substr(0, (*it).length());
		if (str_is_num(port))
		{	if (strtod(port.c_str(), &str) > 65535)
				throw Parse_error(std::string("") + "port [ " + port + " ] is  out range.");
			continue ;
		}
		throw Parse_error(std::string("") + "port should a number.");
	}
	return ;
};

void Parser::validate_host(values_t values){
	std::string err;
	if (_currentBlock == LOCATION)
		throw Parse_error(err + "unexpected host directive in location block");
	if (values.size() != 1)
		throw Parse_error(err + "too many values in host directive");
	std::string ss = (*values.begin());
	std::istringstream iss(ss);
	std::string oct;
	char *str;
	int n = 0;
	while (std::getline(iss, oct, '.') && n < 5)
	{
		if (!str_is_num(oct))
			throw Parse_error(err + "invalid ip address [ " + ss + "]");
		if (strtod(oct.c_str(), &str) > 255)
			throw Parse_error(err + "invalid ip address [ " + ss + "]");
		n++;
	}
	if (n != 4 || !isdigit(ss[ss.length() - 1]))
		throw Parse_error(err + "invalid ip address [ " + ss + "]");
	return ;
}

void Parser::validate_error_page(values_t	values){
	std::string err;
	if (_currentBlock == LOCATION)
		throw Parse_error(err + "unexpected allow directive in location block");
	if (values.size() != 2) 
		throw Parse_error(err + "error_page take at least two values [code/page].");
};

void Parser::validate_server_name(values_t	values){
	std::string err;
	if (_currentBlock == LOCATION)
		throw Parse_error(err + "unexpected server_name directive in location block");
	values_it it = values.begin();
	for (; it != values.end(); it++)
		for (size_s j = 0; j < it->length(); j++)
			if ((*it)[j] != '.' && (*it)[j] != '_' && !isalnum((*it)[j]))
				throw Parse_error(err + "server name should be [0-9a-Z]");
};

void Parser::validate_max(values_t values){
	std::string err;
	if (_currentBlock == LOCATION)
		throw Parse_error(err + "unexpected server_name directive in location block");
	if (values.size() != 1) 
		throw Parse_error(err + "client_max_body_size should have only one value");
	value_t val = *(values.begin());
	if (val.length() > 1 && val[val.length() - 1] == 'M' 
		&& str_is_num(val.substr(0, val.length() - 1)))
		return ;
	throw Parse_error(err + "client_max_body_size should be a number followed by M.");
};

void Parser::validate_index(values_t		values){
	(void)values;
	std::string err;
	if (_currentBlock == SERVER)
		throw Parse_error(err + "unexpected index directive in server block");
};


void Parser::validate_root(values_t			values){
	std::string err;
	if (_currentBlock == SERVER)
		throw Parse_error(err + "unexpected root directive in server block");
	if (values.size() != 1) 
		throw Parse_error(err + "root should have only one value");
};
void Parser::validate_autoindex(values_t	values){
	std::string err;
	if (_currentBlock == SERVER)
		throw Parse_error(err + "unexpected autoindex directive in server block");
	if (values.size() != 1)
		throw Parse_error(err + "too many values in autoindex directive");
	value_t val = *(values.begin());
	if (val != "on" && val != "off")
		throw  Parse_error(err + "unkonwn vaue [ " + val + " ] in autoindex directive");
};

void Parser::validate_cgi(values_t			values){
	(void)values;
	std::string err;
	if (_currentBlock == SERVER)
		throw Parse_error(err + "unexpected autoindex directive in server block");
};	
void Parser::validate_allow(values_t		values){
	std::string err;
	if (_currentBlock == SERVER)
		throw Parse_error(err + "unexpected allow directive in server block");
	values_it it = values.begin();
	for (; it != values.end(); it++)
	{
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			throw Parse_error(err + "unkonwn " + *it + " method.");
	}
};
void Parser::validate_return(values_t		values){
	std::string err;
	if (_currentBlock == SERVER)
		throw Parse_error(err + "unexpected allow directive in server block");
	if (values.size() != 2) 
		throw Parse_error(err + "return take at least two values [code/page].");
};

void Parser::validate_cgi_allow(values_t	values){
		std::string err;
	if (_currentBlock == SERVER)
		throw Parse_error(err + "unexpected cgi_allow directive in server block");
	values_it it = values.begin();
	for (; it != values.end(); it++)
	{
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			throw Parse_error(err + "unkonwn " + *it + " method.");
	}
};

// OH, YOU JUST CAN'T WRITE ANY GARBAGE TEXT
directive_t	Parser::validate(directive_t d)
{
	Key key = getKey(d.first);
	switch (key)
	{
		case MAX : 			validate_max(d.second); 		break;
		case LISTEN: 		validate_listen(d.second); 		break;
		case HOST:			validate_host(d.second);		break;
		case SERVER_NAME:	validate_server_name(d.second); break;
		case INDEX: 		validate_index(d.second); 		break;
		case ERROR_PAGE: 	validate_error_page(d.second); 	break;
		case ROOT: 			validate_root(d.second); 		break;
		case AUTOINDEX: 	validate_autoindex(d.second); 	break;
		case CGI: 			validate_cgi(d.second);			break;
		case ALLOW:			validate_allow(d.second);		break;
		case RETURN:		validate_return(d.second);		break;
		case CGI_ALLOW:		validate_cgi_allow(d.second);	break;
		default: break;
	}
	
	return d;
}
