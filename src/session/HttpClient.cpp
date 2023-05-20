#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpClient.hpp"
#include "HttpMethodProcessor.hpp"
void HttpClient::processRequest(/*HttpRequest& req*/) {
  HttpMethodProcessor method;
  if (req.getMethod() == "GET") {
    res = method.processGetRequest(this->req /* ,configObj*/);
  } else if (req.getMethod() == "POST") {
    res = method.processPostRequest(this->req /* ,configObj*/);
  } else if (req.getMethod() == "DELETE") {
    res = method.processDeleteRequest(this->req /* ,configObj*/);
  } else {
    std::cout <<  "Handle unsupported HTTP method" << std::endl;
  }
  send();
  //or sendChunk();
  //or sendBuffer();
}

void HttpClient::send(){
  std::cout << "not implemented yet" << std::endl;
}
