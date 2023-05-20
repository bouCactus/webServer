#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpClient.hpp"
#include "HttpMethodProcessor.hpp"
#include <unistd.h>
#include <netinet/in.h>

void HttpClient::processRequest(/*HttpRequest& req*/) {
  HttpMethodProcessor method;

  std::cout << req.getMethod() << "---------" << std::endl;
  if (req.getMethod() == "GET") {
    res = method.processGetRequest(this->req /* ,configObj*/);
  } else if (req.getMethod() == "POST") {
    res = method.processPostRequest(this->req /* ,configObj*/);
  } else if (req.getMethod() == "DELETE") {
    res = method.processDeleteRequest(this->req /* ,configObj*/);
  } else {
    std::cout <<  "Handle unsupported HTTP method" << std::endl;
  }
  this->sendit();
  //or sendChunk();
  //or sendBuffer();
}

void HttpClient::sendit(){
  send(_socket, res.getBody().c_str(), res.getBodySize(), 0);
  std::cout << "response sent..." << std::endl;
}

HttpClient::HttpClient(int socket){
  _socket = socket;
}
