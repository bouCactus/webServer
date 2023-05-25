#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpClient.hpp"
#include "HttpMethodProcessor.hpp"
#include <unistd.h>
#include <netinet/in.h>


void HttpClient::processRequest(servers_it& conf_S) {
  HttpMethodProcessor method;

  std::cout << "----------" << req.getMethod() << "---------" << std::endl;
  std::cout << "-------response start from here----------" << std::endl;

  if (req.getMethod() == "GET") {
    res = method.processGetRequest(this->req , conf_S);
    std::cout << "the end of GET" << std::endl;
  } else if (req.getMethod() == "POST") {
    res = method.processPostRequest(this->req , conf_S);
  } else if (req.getMethod() == "DELETE") {
    res = method.processDeleteRequest(this->req , conf_S);
  } else {
    std::cout <<  "Handle unsupported HTTP method" << std::endl;
  }
  this->sendit();
  std::cout << "the end" << std::endl;
  //or sendChunk();
  //or sendBuffer();
}

void HttpClient::sendit(){ //This is unstable, so use it carefully because it might change at any time.
  std::stringstream st;
 
  st << res.getVersion() << " " << res.getStatus() << " " << res.status.getStatusMessage(res.getStatus()) << "\r\n"
     << res.getHeaders() << "\r\n";
  std::cout << st.str() << std::endl;
  send(_socket, st.str().c_str(), st.str().size(), 0); 
  send(_socket, res.getBody().c_str(), res.getBodySize(), 0);
  std::cout << "response sent..." << std::endl;
}

HttpClient::HttpClient(int socket){
  _socket = socket;
}
