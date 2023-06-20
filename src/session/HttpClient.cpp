#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpClient.hpp"
#include "HttpMethodProcessor.hpp"
#include <unistd.h>
#include <netinet/in.h>
#include <fstream>
void sendHeader(HttpResponse& res,int  _socket /*_socket to not complicated things */){
  std::stringstream	st;
  std::string		content;
  st << res.getVersion() << " " << res.getStatus() << " " << res.status.getStatusMessage(res.getStatus()) << "\r\n"
     << res.getHeaders() << "\r\n";
  content = st.str();
  send(_socket, content.c_str(), content.size(), 0);
}

void HttpClient::sendFileResponse(HttpResponse& res, int _socket/*just to test*/)
{
  //here where you open file and send it  by chunks 
 
  std::ifstream file(res.getFilename().c_str(), std::istream::in);
  char		buffer[1024];
  int		sizePos;
  if (!file.is_open()){
    res.setBody("error open file");	// you should replace with error page
      send(_socket, res.getBody().c_str(), res.getBody().size(), 0);      
  }
  if (!_isHeaderSent){
    sendHeader(res, _socket);
    _isHeaderSent = true;
  }
  file.seekg(_writingPos);
  std::streamsize	size = 1024;
  sizePos		     = file.read(buffer, size).gcount();
  if (sizePos <= 0){
    close(_socket);
    _isHeaderSent	     = false;
    _writingPos		     = 0;
  }
  std::cout << buffer << std::endl;
  send(_socket, buffer, sizePos, 0);
  _writingPos += sizePos;
  
}

void HttpClient::processRequest(servers_it& conf_S) {
  HttpMethodProcessor	method;
  std::cout << _isHeaderSent << std::endl;
  if (!_isHeaderSent){
    std::cout << "----------" << req.getMethod() << "---------" << std::endl;
    std::cout << "-------GET start from here----------" << std::endl;

    if (req.getMethod()	       == "GET") {
      res			= method.processGetRequest(this->req , conf_S);
    } else if (req.getMethod() == "POST") {
      res			= method.processPostRequest(this->req , conf_S);
    } else if (req.getMethod() == "DELETE") {
      res			= method.processDeleteRequest(this->req , conf_S);
    } else {
      std::cout <<  "Handle unsupported HTTP method" << std::endl;
    }
  }
  if (res.getBody().empty()){
    std::cout << "ready to send file "<< _writingPos << std::endl;
    sendFileResponse(res,_socket);
    std::cout << "_writingPos" << _writingPos << std::endl;
    while (_writingPos > 0){
    std::cout << "_writingPos" << _writingPos << std::endl;
    sendFileResponse(res,_socket);
    }

     
  }else{
    std::cout << "normal sendResponse" << std::endl;
    sendResponse();
  }
  std::cout << "the end" << std::endl;
}

void HttpClient::sendResponse(){	//This is unstable, so use it carefully because it might change at any time.
  std::cout << "-------sending....---------------" << std::endl;
  sendHeader(res, _socket);
  _isHeaderSent = true;
  send(_socket, res.getBody().c_str(), res.getBodySize(), 0);
  close(_socket);
  _isHeaderSent = false;
  std::cout << "response sent..." << std::endl;
}

HttpClient::HttpClient(int socket):_writing(false),
				   _isHeaderSent(false),
				   _writingPos(0){
  _socket = socket;
}

