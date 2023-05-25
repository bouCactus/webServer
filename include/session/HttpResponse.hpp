#ifndef __HTTPRESPONSE__H_
#define __HTTPRESPONSE__H_

#include "HttpRequest.hpp"
#include "HttpStatusCodes.hpp"
#include <map>
#include <unistd.h>
#include <netinet/in.h>
class HttpResponse{
public:
  HttpStatusCodes status;

  HttpResponse(void);
  HttpResponse(int statusCode);
  HttpResponse(const HttpResponse &other);
  HttpResponse &operator=(const HttpResponse &other);
  ~HttpResponse(void);
  void appendHeader(std::string name, std::string value);
  void setVersion(const std::string version);
  void setStatus(const int status);
  void setBody(std::string body);
  void sendFile(const std::string path);
  void setDate(const std::string date);
  int  getBodySize();
  std::string getBody();
  int  getStatus();
  std::string getVersion();
  std::string getHeaders();
  std::string errorResponse(std::string version ,int status);
 
private:
  
  std::map<std::string, std::string> _headers;
  int _status;
  std::string _version;
  std::string _date;
  std::string _body;

};
#endif // __THHPRESPONSE__H_


// implementation

