#include "HttpResponse.hpp"
#include "HttpStatusCodes.hpp"
#include "fileSystem.hpp"
#include <sstream>

HttpResponse::~HttpResponse(void){
//   std::cout << "response: destructor not implemented yet" << std::endl;
}

HttpResponse::HttpResponse(const HttpResponse &other){
    // std::cout << "Response: copy consturctor not implemented yet" << std::endl;
    *this = other;
}
HttpResponse &HttpResponse::operator = (const HttpResponse &other){
  if (this != &other) {
    _headers			     = other._headers;
    _fileName			     = other._fileName;
    _status			     = other._status;
    _version			     = other._version;
    _date			     = other._date;
    _body			     = other._body;
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
}				// i don't know yet


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
  std::stringstream headerStream;
  for (std::map<std::string, std::string>::iterator it = _headers.begin();
       it != _headers.end() ; it++){
    headerStream << it->first << ": " << it->second << "\r\n" ;
  }

  return (headerStream.str());
}

void HttpResponse::defaultErrorResponse(int status){ 
   std::stringstream	st;
   std::string		body;
  this->setStatus(status);
  this->setVersion("HTTP/1.1");
  this->appendHeader("Content-Type","text/html");

  
  st << "<!DOCTYPE html><html><head><title>"
     << status << " " << this->status.getStatusMessage(status)
     << "</title></head><body><h1><center>"
     << status << " "
     << this->status.getStatusMessage(status)
     << "<hr>"
     << "</center></h1><p><center>webServer/0.01</center></p></body></html>";
  body = st.str();
  this->appendHeader("Content-length", TO_STRING(body.size())); 
  this->setBody(body);
}


void HttpResponse::writeHeader(int statu, Smap_t& header){
  Smap_t::iterator	mapIt;
  setVersion("HTTP/1.1");
  setStatus(statu);
  for(mapIt = header.begin(); mapIt != header.end(); mapIt++)
    appendHeader(mapIt->first, mapIt->second);
}


void HttpResponse::setFilename(const http::filesystem::Path path){
  _fileName = path;
}
http::filesystem::Path        HttpResponse::getFilename(){
  return (_fileName);
}
