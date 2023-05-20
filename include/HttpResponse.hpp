#ifndef __HTTPRESPONSE__H_
#define __HTTPRESPONSE__H_

#include "HttpRequest.hpp"
#include "HttpStatusCodes.hpp"
#include <map>
class HttpResponse{
public:

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
  void setSocket(const int socketId);
  void send();
  void sendchanck();
  void sendBuffer(std::string data, size_t size);
  void end();


  

  // void send_response(int new_socket, HttpResponse response);
private:
  HttpStatusCodes status;
  std::map<std::string, std::string> _headers;
  std::string _status;
  std::string _version;
  std::string _date;
  int         _socketId;
  std::string _body;

};
#endif // __THHPRESPONSE__H_


// implementation

