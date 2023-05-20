#include "../../include/HttpRequest.hpp"

HttpRequest::HttpRequest(void){
  std::cout << "Resquest: constructor not implemented yet" << std::endl;
};

HttpRequest::~HttpRequest(void){
  std::cout << "Resquest: destructor not implemented yet" << std::endl;
}

HttpRequest::HttpRequest(const HttpRequest &other){
  std::cout << "Resquest: copy consturctor not implemented yet" << std::endl;
  (void)other;
}
HttpRequest &HttpRequest::operator=(const HttpRequest &other){
  std::cout << "Request: copy assignment not implemented yet" << std::endl;
  (void)other;
  return (*this);
}

