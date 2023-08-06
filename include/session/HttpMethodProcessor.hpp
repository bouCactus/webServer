#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "confAST.hpp"
#include "HttpClient.hpp"

class HttpMethodProcessor{
public:
  
  void processGetRequest(HttpClient& client, servers_it& conf_S);
  void processPostRequest(HttpClient& client, servers_it& conf_S);
  void processDeleteRequest(HttpClient& client, servers_it& conf_S);

private:

};

#endif // __HTTPSERVER_H__
