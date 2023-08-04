#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "confAST.hpp"
class HttpMethodProcessor{
public:
  
  void processGetRequest(HttpRequest& req , servers_it& conf_S, HttpResponse &res);
  void processPostRequest(HttpRequest& req , servers_it& conf_S, HttpResponse &res);
  void processDeleteRequest(HttpRequest& req , servers_it& conf_S, HttpResponse &res);

private:

};

#endif // __HTTPSERVER_H__
