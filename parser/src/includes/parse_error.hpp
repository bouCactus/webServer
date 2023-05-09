#pragma once
#include <iostream>
#include <exception>

class Parse_error : public std::exception {
	private:
		//const char *_msg;
		std::string *_msg;
		// std::string msg <= this will throw an error, why, i ahev no idea??? do you ?
	public:
		Parse_error(std::string msg): _msg(new std::string(msg)){};
		virtual const char * what () const throw()  {
			//std::cout << "err>> " << *_msg << "\n";
			return _msg->c_str();
		};
		//~Parse_error() {delete _msg;};
};


