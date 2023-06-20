#include "HttpResponse.hpp"
#include "HttpStatusCodes.hpp"

HttpResponse::~HttpResponse(void){
//   std::cout << "response: destructor not implemented yet" << std::endl;
}

HttpResponse::HttpResponse(const HttpResponse &other){
    // std::cout << "Response: copy consturctor not implemented yet" << std::endl;
    *this = other;
}
HttpResponse &HttpResponse::operator=(const HttpResponse &other){
  if (this != &other) {
    _headers = other._headers;
    _status = other._status;
    _version = other._version;
    _date = other._date;
    _body = other._body;
  }
  return *this;
}

HttpResponse::HttpResponse(){
  this->_status = HttpStatusCodes::HttpStatusCodes::HTTP_OK;
}
HttpResponse::HttpResponse(int statusCode){
 this->_status = statusCode;
 this->_version = "HTTP/1.1";
}

void HttpResponse::appendHeader(std::string name, std::string value){
 this->_headers[name] = value;
}

void HttpResponse::setStatus(int status){
  this->_status = status;
}

void HttpResponse::setVersion(const std::string version){
  this->_version = version;
}


void HttpResponse::setDate(const std::string date){
  std::cout << "not implemented yet" << std::endl;
  this->_date = date;
}// i don't know yet


// void HttpResponse::end(){
  // std::cout << "not implemented yet"<<std::endl;

// }

void HttpResponse::setBody(const std::string body){
  _body = body;
}

std::string HttpResponse::getBody(){
  return (_body);
}
int HttpResponse::getBodySize(){
  return (_body.size());
}

std::string HttpResponse::getVersion(){
  return (_version);
}
int HttpResponse::getStatus(){
  return (_status);
}

std::string HttpResponse::getHeaders(){
  std::string head;
  for (std::map<std::string, std::string>::iterator it = _headers.begin() ; it != _headers.end() ; it++){
    head.append(it->first);
    head.append(": ");
    head.append(it->second);
    head.append("\r\n");
  }
  return (head);
}

std::string HttpResponse::errorResponse(std::string version, int status){ 
   std::stringstream st;

  st << version << " " << status << " " << this->status.getStatusMessage(status) << "\r\n"
     << "Content-Type: text/html" << "\r\n"
     << "<!DOCTYPE html><html><head><title>"
     << status << this->status.getStatusMessage(status)
     << "</title></head><body><h1>"
     << this->status.getStatusMessage(status)
     << "</h1><p>server team wish you to find other url</p></body></html>";
  return (st.str());
}