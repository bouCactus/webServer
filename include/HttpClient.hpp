#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpMethodProcessor.hpp"

class HttpClient{

public:
  HttpRequest req;
  HttpResponse res;

  HttpClient(){}; //skiping error ({})
  HttpClient(int socket);
  ~HttpClient(){}; //skiping error ({})
  
  void processRequest(/*HttpRequest& req*/);
  void sendit();
  void sendChunk();
  void sendBuffer();
private:
  int _socket;
  /*
    socket
    config;
   */
};

#endif // __HTTPCLIENT_H__
