
#ifndef __HTTPREQUEST__H_
#define __HTTPREQUEST__H_
#include <sstream>
#include <map>
#include <iostream>
#include "fileSystem.hpp"
/* some headers i found

    CONTENT_LENGTH – The length of the request body (as a string).
    CONTENT_TYPE – The MIME type of the request body.
    HTTP_ACCEPT – Acceptable content types for the response.
    HTTP_ACCEPT_ENCODING – Acceptable encodings for the response.
    HTTP_ACCEPT_LANGUAGE – Acceptable languages for the response.
    HTTP_HOST – The HTTP Host header sent by the client.
    HTTP_REFERER – The referring page, if any.
    HTTP_USER_AGENT – The client’s user-agent string.
    QUERY_STRING – The query string, as a single (unparsed) string.
    REMOTE_ADDR – The IP address of the client.
    REMOTE_HOST – The hostname of the client.
    REMOTE_USER – The user authenticated by the web server, if any.
    REQUEST_METHOD – A string such as "GET" or "POST".
    SERVER_NAME – The hostname of the server.
    SERVER_PORT – The port of the server (as a string).

 */
class HttpRequest{
public:
  HttpRequest();
  HttpRequest(const HttpRequest &other);
  HttpRequest &operator=(const HttpRequest &other);
  ~HttpRequest();

  void parser(std::string rawData){
    std::stringstream ss(rawData);
    std::string path;
    ss >> this->_method >> path >> _version ;
    _path.setPath(path);
    std::cout << "parser get path" << std::endl;
  }
  void printRaw(){
    std::cout <<  this->_method << " - " << this->_path.c_str() << " - " << this->_version<< std::endl;
  }
  std::string getMethod(){
    return (_method);
  }
  http::filesystem::Path getPath(){
    return (_path);
  }
  std::string getVersion(){
    return (_version);
  }
private:
  std::string _method;
  http::filesystem::Path _path; // this a class which deal with proning staff of path
  std::string _version;
  std::map<std::string, std::string> headers;

};



#endif //__HTTPREQUEST__H_


