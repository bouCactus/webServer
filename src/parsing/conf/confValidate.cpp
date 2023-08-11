#include "confParseError.hpp"
#include "confParser.hpp"
#include "confTypes.hpp"
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

/**
 * @brief   		get the corresponding Key of a directive.
 * @param	key	 	the string represent a key of a directive.
 * @return 			the corresponding Key (enum) to key(string).
 */
Key getKey(std::string key)
{
  if (key == "client_max_body_size")
    return MAX;
  if (key == "listen")
    return LISTEN;
  if (key == "server_name")
    return SERVER_NAME;
  if (key == "index")
    return INDEX;
  if (key == "error_page")
    return ERROR_PAGE;
  if (key == "root")
    return ROOT;
  if (key == "autoindex")
    return AUTOINDEX;
  if (key == "cgi_pass")
    return CGI;
  if (key == "host")
    return HOST;
  if (key == "allow")
    return ALLOW;
  if (key == "return")
    return RETURN;
  if (key == "cgi_allow")
    return CGI_ALLOW;
  if (key == "upload_path")
    return UPLOAD_PATH;
  if (key == "cgi_time_out")
    return CGI_TIME_OUT;
  {
    LOG_THROW();
    throw Parse_error(std::string("") + "unkown directive " + key);
  }
}

/**
 * @brief   		check if s is representing a valid number.
 * @param	s	 	the string to be checked.
 * @return 			bool indicates wither s is num ot not.
 */
int str_is_num(std::string s)
{
  for (size_s i = 0; i < s.size(); i++)
    if (!isdigit(s[i]))
      return 0;
  return !s.size() ? 0 : 1;
}

/**
 * @brief   		validate a listen directive.
 * @param	values	set of string values.
 */
void Parser::validate_listen(values_t values)
{
  std::string err;

  if (_currentBlock == LOCATION)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected listen directive in location block");
  }
  values_it it = values.begin();
  for (; it != values.end(); it++)
  {
    char *str;
    std::string port = (*it).substr(0, (*it).length());
    if (str_is_num(port))
    {
      if (strtod(port.c_str(), &str) > 65535)
      {
        LOG_THROW();
        throw Parse_error(std::string("") + "port [ " + port +
                          " ] is  out range.");
      }
      continue;
    }
    {
      LOG_THROW();
      throw Parse_error(std::string("") + "port should a number.");
    }
  }
  return;
};

/**
 * @brief   		validate a host directive.
 * @param	values	set of string values.
 */
void Parser::validate_host(values_t values)
{
  std::string err;
  if (_currentBlock == LOCATION)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected host directive in location block");
  }
  if (values.size() != 1)
  {
    LOG_THROW();
    throw Parse_error(err + "too many values in host directive");
  }
  std::string ss = (*values.begin());
  std::istringstream iss(ss);
  std::string oct;
  char *str;
  int n = 0;
  while (std::getline(iss, oct, '.') && n < 5)
  {
    if (!str_is_num(oct))
    {
      LOG_THROW();
      throw Parse_error(err + "invalid ip address [ " + ss + "]");
    }
    if (strtod(oct.c_str(), &str) > 255)
    {
      LOG_THROW();
      throw Parse_error(err + "invalid ip address [ " + ss + "]");
    }
    n++;
  }
  if (n != 4 || !isdigit(ss[ss.length() - 1]))
  {
    LOG_THROW();
    throw Parse_error(err + "invalid ip address [ " + ss + "]");
  }
  return;
}

/**
 * @brief   		validate a error_page directive.
 * @param	values	set of string values.
 */

void Parser::validate_error_page(values_t values)
{
  std::string err;
  if (_currentBlock == LOCATION)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected allow directive in location block");
  }
  if (values.size() != 2)
  {
    LOG_THROW();
    throw Parse_error(err + "error_page take at least two values [code/page].");
  }
  int status = std::atoi((*values.begin()).c_str());
  if ((*values.begin()).length() > 3 || status < 400 || status > 511 || (status > 451 && status < 500))
  {
    LOG_THROW();
    throw Parse_error(err + "the status code shoud be a valid integer [400-451 | 500-511].");
  }
  int fd = open((*(++values.begin())).c_str(), O_RDONLY);
  if (fd == -1)
  {
    LOG_THROW();
    throw Parse_error(err + "The error page [" + (*values.begin()) + "/" + (*++values.begin()) + "is not avilable for reading.");
  }
  close(fd);
};

/**
 * @brief   		validate a server_name directive.
 * @param	values	set of string values.
 */
void Parser::validate_server_name(values_t values)
{
  std::string err;
  if (_currentBlock == LOCATION)
  {
    LOG_THROW();
    throw Parse_error(err +
                      "unexpected server_name directive in location block");
  }
  values_it it = values.begin();
  for (; it != values.end(); it++)
    for (size_s j = 0; j < it->length(); j++)
      if ((*it)[j] != '.' && (*it)[j] != '_' && !isalnum((*it)[j]))
      {
        LOG_THROW();
        throw Parse_error(err + "server name should be [0-9a-Z]");
      }
};

/**
 * @brief   		validate a client_max_body_size directive.
 * @param	values	set of string values.
 */
void Parser::validate_max(values_t values)
{
  std::string err;
  if (_currentBlock == LOCATION)
  {
    LOG_THROW();
    throw Parse_error(err +
                      "unexpected server_name directive in location block");
  }
  if (values.size() != 1)
  {
    LOG_THROW();
    throw Parse_error(err + "client_max_body_size should have only one value");
  }
  value_t val = *(values.begin());
  if (val.length() > 1 && val[val.length() - 1] == 'M')
  {
    int max = std::atoi(val.substr(0, val.length() - 1).c_str());
    if (val.length() > 7 || max > 100000 || max == 0)
    {
      LOG_THROW();
      throw Parse_error(err + "client_max_body_size should have value between [1 - 100000M]");
    }
    return;
  }
  {
    LOG_THROW();
    throw Parse_error(err +
                      "client_max_body_size should be a number followed by M.");
  }
};

/**
 * @brief   		validate a index directive.
 * @param	values	set of string values.
 */
void Parser::validate_index(values_t values)
{
  (void)values;
  std::string err;
  if (_currentBlock == SERVER)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected index directive in server block");
  }
};

/**
 * @brief   		validate a root directive.
 * @param	values	set of string values.
 */
void Parser::validate_root(values_t values)
{
  std::string err;
  if (_currentBlock == SERVER)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected root directive in server block");
  }
  if (values.size() != 1)
  {
    LOG_THROW();
    throw Parse_error(err + "root should have only one value");
  }
};

/**
 * @brief   		validate a autoindex directive.
 * @param	values	set of string values.
 */
void Parser::validate_autoindex(values_t values)
{
  std::string err;
  if (_currentBlock == SERVER)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected autoindex directive in server block");
  }
  if (values.size() != 1)
  {
    LOG_THROW();
    throw Parse_error(err + "too many values in autoindex directive");
  }
  value_t val = *(values.begin());
  if (val != "on" && val != "off")
  {
    LOG_THROW();
    throw Parse_error(err + "unkonwn vaue [ " + val +
                      " ] in autoindex directive");
  }
};

/**
 * @brief   		validate a cgi_pass directive.
 * @param	values	set of string values.
 */
void Parser::validate_cgi(values_t values)
{
  (void)values;
  std::string err;
  if (_currentBlock == SERVER)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected autoindex directive in server block");
  }

  // check if the syntax of cgi lists are correct!!

  // if (values.size() != 1)
  // {
  // 	{ LOG_THROW(); throw Parse_error("cgi_pass should have only one
  // value."); }
  // }
  // values_it it = values.begin();
  // if (*it != "phpfpm")
  // {
  // 	LOG_THROW(); throw Parse_error("CGI is not allowed!");
  // }
};

/**
 * @brief   		validate a allow directive.
 * @param	values	set of string values.
 */
void Parser::validate_allow(values_t values)
{
  std::string err;
  if (_currentBlock == SERVER)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected allow directive in server block");
  }
  values_it it = values.begin();
  for (; it != values.end(); it++)
  {
    if (*it != "GET" && *it != "POST" && *it != "DELETE")
    {
      LOG_THROW();
      throw Parse_error(err + "unkonwn " + *it + " method.");
    }
  }
};

/**
 * @brief   		validate a return directive.
 * @param	values	set of string values.
 */
void Parser::validate_return(values_t values)
{
  std::string err;
  if (_currentBlock == SERVER)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected allow directive in server block");
  }
  if (values.size() != 2)
  {
    LOG_THROW();
    throw Parse_error(err + "return take at least two values [code/page].");
  }
  int status = std::atoi((*values.begin()).c_str());
  if ((*values.begin()).length() > 3 || status < 300 || status > 308)
  {
    LOG_THROW();
    throw Parse_error(err + "redirection status code should be valid integer [300 - 308].");
  }
};

/**
 * @brief   		validate a cgi_allow directive.
 * @param	values	set of string values.
 */
void Parser::validate_cgi_allow(values_t values)
{
  std::string err;
  if (_currentBlock == SERVER)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected cgi_allow directive in server block");
  }
  values_it it = values.begin();
  for (; it != values.end(); it++)
  {
    if (*it != "GET" && *it != "POST")
    {
      LOG_THROW();
      throw Parse_error(err + "unkonwn " + *it + " method.");
    }
  }
};

void Parser::validate_upload_path(values_t values)
{
  std::string err;
  if (_currentBlock == SERVER)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected allow directive in server block");
  }
  if (values.size() != 1)
  {
    LOG_THROW();
    throw Parse_error(err + "upload path take at least one value [path].");
  }
}

void Parser::validate_cgi_time_out(values_t values)
{
  std::string err;
  if (_currentBlock == SERVER)
  {
    LOG_THROW();
    throw Parse_error(err + "unexpected allow directive in server block");
  }

  int s = atoi(values.begin()->c_str());
  if (values.size() != 1 || values.begin()->size() > 4 || s > 3600 || s < 1)
  {
    LOG_THROW();
    throw Parse_error(err + "cgi_time take a number in seconds [1s-3600s].");
  }
};

/**
 * @brief   		validate a cgi_pass directive.
 * @param	d		pair of value_t and values_t representing
 * 					a directive to be virified.
 * @return 			the same directive passed as parameter.
 */
directive_t Parser::validate(directive_t d)
{
  Key key = getKey(d.first);
  switch (key)
  {
  case MAX:
    validate_max(d.second);
    break;
  case LISTEN:
    validate_listen(d.second);
    break;
  case HOST:
    validate_host(d.second);
    break;
  case SERVER_NAME:
    validate_server_name(d.second);
    break;
  case INDEX:
    validate_index(d.second);
    break;
  case ERROR_PAGE:
    validate_error_page(d.second);
    break;
  case ROOT:
    validate_root(d.second);
    break;
  case AUTOINDEX:
    validate_autoindex(d.second);
    break;
  case CGI:
    validate_cgi(d.second);
    break;
  case ALLOW:
    validate_allow(d.second);
    break;
  case RETURN:
    validate_return(d.second);
    break;
  case CGI_ALLOW:
    validate_cgi_allow(d.second);
    break;
  case UPLOAD_PATH:
    validate_upload_path(d.second);
    break;
  case CGI_TIME_OUT:
    validate_cgi_time_out(d.second);
    break;
  default:
    break;
  }

  return d;
}
