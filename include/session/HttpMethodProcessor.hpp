#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "confAST.hpp"
class HttpMethodProcessor{
public:
  
  HttpResponse processGetRequest(HttpRequest& req , servers_it& conf_S);
  HttpResponse processPostRequest(HttpRequest& req , servers_it& conf_S);
  HttpResponse processDeleteRequest(HttpRequest& req , servers_it& conf_S);

private:

};

#endif // __HTTPSERVER_H__
