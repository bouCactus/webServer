#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpMethodProcessor.hpp"
#include "confAST.hpp"

class HttpClient{

public:
  HttpRequest req;
  HttpResponse res;

  HttpClient(){}; //skiping error ({})
  HttpClient(int socket);
  ~HttpClient(){}; //skiping error ({})
  
  void processRequest(servers_it& conf_S);
  void sendit();
  void sendChunk();
  void sendBuffer();
private:
  int _socket;
  
};

#endif // __HTTPCLIENT_H__
