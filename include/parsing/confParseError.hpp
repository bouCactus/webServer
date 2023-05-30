#pragma once
#include <iostream>
#include <exception>

class Parse_error : public std::exception {
	private:
		std::string *_msg;
	public:
		Parse_error(std::string msg): _msg(new std::string(msg)){};
		virtual const char * what () const throw()  {
			return _msg->c_str();
		};
};


