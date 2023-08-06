#pragma once

#include "./confAST.hpp"
#include "./confLexer.hpp"
#include "./confParseError.hpp"
#include "./confTypes.hpp"
#include <iostream>

#include <map>
#include <vector>

class Parser {
private:
  Lexer *_lxr;
  Type _currentBlock;
  Parser(Parser &){};
  Parser &operator=(Parser &p) { return p; };
  servers_t servers;
  locations_it _currentLocation;

public:
  Parser(std::string path);
  servers_t operator()();
  void parse_block(Token t);
  directive_t parse_directive();
  directive_t validate(directive_t d);
  void validate_max(values_t values);
  void validate_listen(values_t values);
  void validate_server_name(values_t values);
  void validate_index(values_t values);
  void validate_error_page(values_t values);
  void validate_root(values_t values);
  void validate_autoindex(values_t values);
  void validate_cgi(values_t values);
  void validate_host(values_t values);
  void validate_allow(values_t values);
  void validate_return(values_t values);
  void validate_cgi_allow(values_t values);

  ~Parser();
};
