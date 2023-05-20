#include "HttpMethodProcessor.hpp"
#include "HttpRequest.hpp"
#include "HttpRequest.hpp"

HttpResponse HttpMethodProcessor::processGetRequest(HttpRequest& req /* configObj*/) {
  std::cout << "GET request not implemented "<< std::endl;
  (void)req;
  HttpResponse res(200);
  res.appendHeader("Content-type" , "text/html");
  res.appendHeader("Connection" ,"keep-alive");
  res.setBody("<html> <body>error 404</body></html>");
  return (res);
    
}

HttpResponse HttpMethodProcessor::processPostRequest(HttpRequest& req /* ,configObj*/) {
  std::cout << "POST request not implemented "<< std::endl;
  HttpResponse res;
  (void)req;
  return (res);// for skiping error

}

HttpResponse HttpMethodProcessor::processDeleteRequest(HttpRequest& req /*, configObj*/) {
  std::cout << "DELETE request not implemented "<< std::endl;

  (void)req;
  HttpResponse res;
  return (res);// for skiping error

}


