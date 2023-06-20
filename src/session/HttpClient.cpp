#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpClient.hpp"
#include "HttpMethodProcessor.hpp"
#include <unistd.h>
#include <netinet/in.h>

// HttpClient::HttpClient(){
    
// }
HttpClient::HttpClient(const HttpClient& httpClient) {

    // std::cout << ">>> Copy Constructor called\n";
    this->_socket = httpClient.getSocket();
    this->conf = new Server(*(httpClient.getConfiguration()));
}

HttpClient& HttpClient::operator=(const HttpClient& httpClient) {
    // std::cout << ">>> Assignement operator called\n";
    if (this != &httpClient) {
        this->_socket = httpClient.getSocket();
        this->conf = new Server(*(httpClient.getConfiguration()));

    }
	return *this;
}


HttpClient::HttpClient(Server *server, int socket) : _socket(socket) {
	this->conf = server;
    _sendFinished = false;
}

HttpClient::~HttpClient(){
    delete conf;
    // std::cout << ">>> Destructor called\n";
}

/**********************************************************/
/**************** Handle Request Functions ****************/
/**********************************************************/

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


/**********************************************************/
/************* Client Configuration Functions *************/
/**********************************************************/

Server*	HttpClient::getConfiguration() const {return conf;}
int		HttpClient::getSocket() const {return _socket;}

// void	HttpClient::setConfiguration(Server& server) {conf = server;}
void	HttpClient::set_socket(int clientSocket) {_socket = clientSocket;}

// void	HttpClient::set_Reading_State(bool state) {_readyTo_Read = state;}
// void	HttpClient::set_Writing_State(bool state) {_readyTo_Write = state;}
// bool	HttpClient::ReadyTo_Read() {return _readyTo_Read;}
// bool	HttpClient::ReadyTo_Write() {return _readyTo_Write;}


void	HttpClient::set_ToFinish(bool state) {_sendFinished = state;}
bool	HttpClient::sendIsFinished() {return _sendFinished;}