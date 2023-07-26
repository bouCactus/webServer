#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpClient.hpp"
#include "HttpMethodProcessor.hpp"
#include <unistd.h>
#include <netinet/in.h>

#include <fstream>
int sendHeader(HttpResponse& res,int  _socket /*_socket to not complicated things */){
  std::stringstream	st;
  std::string		content;

  st << res.getVersion() << " " << res.getStatus() << " " << res.status.getStatusMessage(res.getStatus()) << "\r\n"
     << res.getHeaders() << "\r\n";
  content = st.str();
  return send(_socket, content.c_str(), content.size(), 0);
}



HttpClient::HttpClient(const HttpClient& httpClient) {

    std::cout << "HttpClient:: >>> Copy Constructor called\n";
    this->_socket = httpClient.getSocket();
    _conf = httpClient._conf;
    _isRequestComplete = false;
}

HttpClient& HttpClient::operator=(const HttpClient& httpClient) {
    std::cout << "HttpClient::>>> Assignement operator called\n";
    if (this != &httpClient) {
        this->_socket = httpClient.getSocket();
         _conf = httpClient._conf;
    }
	return *this;
}


HttpClient::HttpClient(servers_it& server, int socket) : _socket(socket) {
	  _conf = server;
    _isRespondComplete = false;
    _writing = false;
    _isHeaderSent = false;
    _writingPos = 0;
    
}

HttpClient::~HttpClient(){

}


int HttpClient::sendFileResponse(HttpResponse& res, int socket/*just to test*/)
{
  //here where you open file and send it  by chunks 
  
  std::ifstream file(res.getFilename().c_str(), std::istream::in);
  char		buffer[1025];
  int		sizePos;
  int bytesSent;
  if (!file.is_open()){
    res.setBody("error open file");	// you should replace with error page
    bytesSent =  send(socket, res.getBody().c_str(), res.getBody().size(), 0);      
  }
  if (!_isHeaderSent){
    bytesSent = sendHeader(res, socket);
    _isHeaderSent = true;
  }
  file.seekg(_writingPos);
  std::streamsize	size = 1024;
  sizePos		     = file.read(buffer, size).gcount();
  buffer[sizePos] = '\0';
  if (sizePos <= 0){
    //close(_socket);
    _isHeaderSent	     = false;
    _writingPos		     = 0;
    _isRespondComplete = true;
  }
  // std::cout << buffer << std::endl;
  bytesSent = send(socket, buffer, sizePos, 0);
  _writingPos += sizePos;
  return (bytesSent);
}

/**********************************************************/
/**************** Handle Request Functions ****************/
/**********************************************************/

void HttpClient::processRequest(servers_it& conf_S) {
  HttpMethodProcessor	method;
  if (!_isHeaderSent){
    if (req.getMethod()	== "GET") {
      res	= method.processGetRequest(this->req , conf_S);
    } else if (req.getMethod() == "POST") {
      res			= method.processPostRequest(this->req , conf_S);
    } else if (req.getMethod() == "DELETE") {
      res			= method.processDeleteRequest(this->req , conf_S);
    } else {
      std::cout <<  "Handle unsupported HTTP method" << std::endl;
    }
  }
  
}

int HttpClient::sendResponse(){	
  std::cout << "-------sending....---------------" << std::endl;
  sendHeader(res, _socket);
  _isHeaderSent = true;
   int bytesSent =  send(_socket, res.getBody().c_str(), res.getBodySize(), 0);
  _isRespondComplete = true;
  _isHeaderSent = false;
  return (bytesSent);
  std::cout << "response sent..." << std::endl;
}

/**********************************************************/
/************* Client Configuration Functions *************/
/**********************************************************/

servers_it	HttpClient::getConfiguration() const {return _conf;}
int		HttpClient::getSocket() const {return _socket;}

void	HttpClient::set_socket(int clientSocket) {_socket = clientSocket;}


void	HttpClient::setRespondComplete(bool state) {_isRespondComplete = state;}
bool	HttpClient::isRespondComplete() {return _isRespondComplete;}

bool 	  HttpClient::isRequestComplete(){
  return (_isRequestComplete);
}
void    HttpClient::setRequestComplete(bool state){
  _isRequestComplete = state;
}