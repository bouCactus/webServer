#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpClient.hpp"
#include "HttpMethodProcessor.hpp"
#include <unistd.h>
#include <netinet/in.h>

HttpClient::HttpClient(){}
HttpClient::HttpClient(const HttpClient& httpClient) {
	*this = httpClient;
}
HttpClient& HttpClient::operator=(const HttpClient& httpClient) {
	(void)(httpClient);
	return *this;
}


HttpClient::HttpClient(servers_it &server, int serverSocket, int socket) {
	this->conf = new Server;
	this->conf = &*server;
	this->_serverSocket = serverSocket;
	this->_socket = socket;
}

HttpClient::~HttpClient(){
	delete this->conf;
}

/**********************************************************/
/**************** Handle Request Functions ****************/
/**********************************************************/

// void HttpClient::processRequest(servers_it& conf_S) {
//   HttpMethodProcessor method;

//   std::cout << "----------" << req.getMethod() << "---------" << std::endl;
//   std::cout << "-------response start from here----------" << std::endl;

//   if (req.getMethod() == "GET") {
//     res = method.processGetRequest(this->req , conf_S);
//     std::cout << "the end of GET" << std::endl;
//   } else if (req.getMethod() == "POST") {
//     res = method.processPostRequest(this->req , conf_S);
//   } else if (req.getMethod() == "DELETE") {
//     res = method.processDeleteRequest(this->req , conf_S);
//   } else {
//     std::cout <<  "Handle unsupported HTTP method" << std::endl;
//   }
//   this->sendit();
//   std::cout << "the end" << std::endl;
//   //or sendChunk();
//   //or sendBuffer();
// }

// void HttpClient::sendit(){ //This is unstable, so use it carefully because it might change at any time.
//   std::stringstream st;
 
//   st << res.getVersion() << " " << res.getStatus() << " " << res.status.getStatusMessage(res.getStatus()) << "\r\n"
//      << res.getHeaders() << "\r\n";
//   std::cout << st.str() << std::endl;
//   send(_socket, st.str().c_str(), st.str().size(), 0); 
//   send(_socket, res.getBody().c_str(), res.getBodySize(), 0);
//   std::cout << "response sent..." << std::endl;
// }


/**********************************************************/
/************* Client Configuration Functions *************/
/**********************************************************/

Server*	HttpClient::getConfiguration() {return conf;}
int		HttpClient::getServerSocket() {return _serverSocket;}
int		HttpClient::getSocket() {return _socket;}

void	HttpClient::setConfiguration(Server* server) {conf = server;}
void	HttpClient::set_socket(int clientSocket) {_socket = clientSocket;}

void	HttpClient::set_Reading_State(bool state) {_readyTo_Read = state;}
void	HttpClient::set_Writing_State(bool state) {_readyTo_Write = state;}
bool	HttpClient::ReadyTo_Read() {return _readyTo_Read;}
bool	HttpClient::ReadyTo_Write() {return _readyTo_Write;}
