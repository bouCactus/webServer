#include "HttpMethodProcessor.hpp"
#include "HttpRequest.hpp"
#include "HttpRequest.hpp"

HttpResponse HttpMethodProcessor::processGetRequest(HttpRequest& req , servers_it& conf_S) {
  std::cout << "GET request not implemented "<< std::endl;
  (void)req;
  (void)conf_S;
  std::cout << http::filesystem::isDirectory(req.getPath()) << std::endl;
    std::cout << http::filesystem::isExests(req.getPath()) << std::endl;
  std::cout << "-----------------------------------"<< std::endl;
  HttpResponse res(200);
  res.setVersion("HTTP/1.1");
  res.appendHeader("Content-type" , "text/html");
  res.appendHeader("Connection" ,"keep-alive");
  res.setBody("<html> <body>error 404</body></html>");
  return (res);
    
}

HttpResponse HttpMethodProcessor::processPostRequest(HttpRequest& req , servers_it& conf_S) {
  std::cout << "POST request not implemented "<< std::endl;
  HttpResponse res;
  (void)req;
   (void)conf_S;
  return (res);// for skiping error

}

HttpResponse HttpMethodProcessor::processDeleteRequest(HttpRequest& req , servers_it& conf_S) {
  std::cout << "DELETE request not implemented "<< std::endl;

  (void)req;
   (void)conf_S;
  HttpResponse res;
  
  return (res);// for skiping error

}


