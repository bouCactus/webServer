#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpMethodProcessor.hpp"
#include "confAST.hpp"
#include <fstream>

class HttpClient{

public:
  HttpRequest req;
  HttpResponse res;

  HttpClient(){
    _writing = false;
    _isHeaderSent = false;
    _writingPos = 0;
  }; //skiping error ({})
  HttpClient(int socket);
  ~HttpClient(){}; //skiping error ({})
  
  void processRequest(servers_it& conf_S);
  void sendResponse();
  void sendFileResponse(HttpResponse& res, int _socket);
private:
  bool _writing;
  bool _isHeaderSent;
  std::streamsize _writingPos;
  int _socket;
  
};

#endif // __HTTPCLIENT_H__
