#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
class HttpMethodProcessor{
public:
  
  HttpResponse processGetRequest(HttpRequest& req);
  HttpResponse processPostRequest(HttpRequest& req);
  HttpResponse processDeleteRequest(HttpRequest& req);

private:

};

#endif // __HTTPSERVER_H__
