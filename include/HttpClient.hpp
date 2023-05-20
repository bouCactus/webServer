#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"


class HttpClient{

public:
  HttpClient(){}; //skiping error ({})
  ~HttpClient(){}; //skiping error ({})
  
  void processRequest(/*HttpRequest& req*/);
  void send();
  void sendChunk();
  void sendBuffer();
private:
  HttpRequest req;
  HttpResponse res;
  /*
    socket
    config;
   */
};

#endif // __HTTPCLIENT_H__
