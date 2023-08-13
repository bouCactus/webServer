#include "HttpResponse.hpp"
#include "HttpStatusCodes.hpp"
#include "fileSystem.hpp"
#include <sstream>
#include "utilsFunction.hpp"
#include <unistd.h>

HttpResponse::~HttpResponse(void){
//   //std::cout << "response: destructor not implemented yet" << std::endl;
  //close(_CGIFile.first);
  //unlink(_CGIFile.second.c_str());
  // //std::cout << "res end here-----*\n";
}

void HttpResponse::clean() {
  // close the temp file of CGI if there is one.
  if (getCGIFile().first != 0)
    close(getCGIFile().first);
  if (!getCGIFile().second.empty())
    unlink(getCGIFile().second.c_str());
}

HttpResponse::HttpResponse(const HttpResponse &other){
    // //std::cout << "Response: copy consturctor not implemented yet" << std::endl;
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
    _CGIFile        = other._CGIFile;
    _proccessPID    = other._proccessPID;
  }
  return *this;
}

void HttpResponse::setCGIFile(int fd, std::string path) {
  _CGIFile.first = fd;
  _CGIFile.second = path;
};

std::pair<int, std::string> HttpResponse::getCGIFile() {
  return _CGIFile;
};

void HttpResponse::setProccessPID(int pid)
{
  _proccessPID = pid;
}

int HttpResponse::getProccessPID() {
  return (_proccessPID);
}

HttpResponse::HttpResponse(){
  this->_status = HttpStatusCodes::HttpStatusCodes::HTTP_OK;
  _proccessPID = -1;
  // the problem was in version this because something happened to response and he lose it
 // i think you call this constructor
 // i add this line 
   this->_version = "HTTP/1.1";
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
  //std::cout << "not implemented yet" << std::endl;
  this->_date = date;
}				// i don't know yet


// void HttpResponse::end(){
  // //std::cout << "not implemented yet"<<std::endl;

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
  this->setStatus(status);
  this->setVersion("HTTP/1.1");
  this->appendHeader("Date", getTimeGMT());
  this->appendHeader("Connection", "keep-alive");
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
