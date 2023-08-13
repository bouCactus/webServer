#include "HttpClient.hpp"
#include "HttpMethodProcessor.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "confAST.hpp"
#include "confTypes.hpp"
#include "fileSystem.hpp"
#include <ctime>
#include <netinet/in.h>
#include <signal.h>
#include <sys/signal.h>
#include <unistd.h>
#include "utilsFunction.hpp"
#include <fstream>
int sendHeader(HttpResponse &res, int _socket ) {
  std::stringstream st;
  std::string content;
  st << res.getVersion() << " " << res.getStatus() << " "
     << res.status.getStatusMessage(res.getStatus()) << "\r\n"
     << res.getHeaders() << "\r\n";
  content = st.str();
  return send(_socket, content.c_str(), content.size(), 0);
}

void HttpClient::setIndexPath(hfs::Path &path) { _indexPath = path; }
hfs::Path &HttpClient::getIndexPath() { return _indexPath; }

HttpClient::HttpClient(const HttpClient &httpClient) {

  this->_socket = httpClient.getSocket();
  _conf = httpClient._conf;
  _isRequestComplete = false;
  _isRespondComplete = false;
}

HttpClient &HttpClient::operator=(const HttpClient &httpClient) {
  if (this != &httpClient) {
    this->_socket = httpClient.getSocket();
    _conf = httpClient._conf;
  }
  return *this;
}

HttpClient::HttpClient(servers_it &server, int socket) : _socket(socket) {
  _conf = server;
  _isRespondComplete = false;
  _writing = false;
  _isHeaderSent = false;
  _writingPos = 0;
  _connectedToServer = true;
}

void HttpClient::clean() {
  if (this->res.getProccessPID() != -1)
    kill(this->res.getProccessPID(), SIGKILL);
  this->res.clean();
}

void HttpClient::clean(std::list<HttpClient*> &clients) {
  if (this->res.getProccessPID() != -1) {
    kill(this->res.getProccessPID(), SIGKILL);
  }
  std::vector<int> clientSockets;
  std::list<HttpClient*>::iterator client = clients.begin();
  for (; client != clients.end(); client++) {
    clientSockets.push_back((*client)->getSocket());
  }
  this->res.clean(clientSockets);
}

HttpClient::~HttpClient() {
  if (this->res.getProccessPID() != -1)
  {
    kill(this->res.getProccessPID(), SIGKILL);
  }
}

int HttpClient::sendFileResponse(HttpResponse &res, int socket ) {
  // here where you open file and send it  by chunks

  std::ifstream file(res.getFilename().c_str(), std::istream::in);
  char buffer[1025];
  int sizePos;
  int bytesSent;
  if (!file.is_open()) {
    res.setBody("error open file"); // you should replace with error page
    bytesSent = send(socket, res.getBody().c_str(), res.getBody().size(), 0);
  }
  if (!_isHeaderSent) {
    serverLog(*this);
    bytesSent = sendHeader(res, socket);
    _isHeaderSent = true;
    return bytesSent;
  }
  file.seekg(_writingPos);
  std::streamsize size = 1024;
  sizePos = file.read(buffer, size).gcount();
  buffer[sizePos] = '\0';
  if (sizePos <= 0) {
    _isHeaderSent = false;
    _writingPos = 0;
    _isRespondComplete = true;
  }
  bytesSent = send(socket, buffer, sizePos, 0);
  _writingPos += sizePos;
  return (bytesSent);
}

/**********************************************************/
/**************** Handle Request Functions ****************/
/**********************************************************/

std::string shouldRedirect(const HttpClient &client,
                           const servers_it &serverConf) {
  http::filesystem::Path reqResouces = client.req.getPath();
  try {
    std::string locationName =
        client.req.findlocationOfUrl(reqResouces, serverConf);
    Location location = serverConf->at(locationName);
    values_t newLocation = location.getRedirect();
    if (newLocation.size() <= 0) {
      return ("");
    }
    return (*(newLocation.begin() + 1)); // replace the string with real value
  } catch (...) {
    return ("");
  }
}

void redirectToNewLocation(std::string newLcoation, HttpClient &client) {
  client.res.defaultErrorResponse(301);
  client.res.appendHeader("Location", newLcoation);
}

bool isLocationAllowMethod(HttpClient& client,
                           const servers_it &serverConf, Req method) {
  HttpRequest& req = client.req;
  std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
  
  try {
    if (serverConf->at(location).isAllowed(method))
      return (true);
  } catch (std::exception &) {
  }
  
  return (false);
}
void HttpClient::processRequest(servers_it &conf_S) {
  HttpMethodProcessor method;
  if (!_isHeaderSent) {
    std::string locationToRedirect = shouldRedirect(*this, conf_S);

    if (!locationToRedirect.empty()) {
      redirectToNewLocation(locationToRedirect, *this);
      return;
    } else if (req.getMethod() == "GET") {
      if (!isLocationAllowMethod(*this, conf_S, GET))
        throw 405;
      method.processGetRequest(*this, conf_S);
    } else if (req.getMethod() == "POST") {
       if (!isLocationAllowMethod(*this, conf_S, POST))
        throw 405;
      method.processPostRequest(*this, conf_S);
    } else if (req.getMethod() == "DELETE") {
       if (!isLocationAllowMethod(*this, conf_S, DELETE))
        throw 405;
      method.processDeleteRequest(*this, conf_S);
    } else {
      throw 405;
    }
  }
}

void HttpClient::setTimeOut(time_t time) { _timeOut = time; };

time_t HttpClient::getTimeOut() const { return _timeOut; };

int HttpClient::sendResponse() {
  serverLog(*this);
 

    std::stringstream st;
  std::string content;
  int bytesSent = -1;
 
  st << res.getVersion() << " " << res.getStatus() << " "
     << res.status.getStatusMessage(res.getStatus()) << "\r\n"
     << res.getHeaders() << "\r\n";
  content = st.str();

  content.append(res.getBody());
  bytesSent = send(_socket, content.c_str(), content.size(), 0);

  _isHeaderSent = true;
  _isRespondComplete = true;
  _isHeaderSent = false;

  return (bytesSent);
}

/**********************************************************/
/************* Client Configuration Functions *************/
/**********************************************************/

servers_it HttpClient::getConfiguration() const { return _conf; }
int HttpClient::getSocket() const { return _socket; }

void HttpClient::set_socket(int clientSocket) { _socket = clientSocket; }

void HttpClient::setRespondComplete(bool state) { _isRespondComplete = state; }
bool HttpClient::isRespondComplete() { return _isRespondComplete; }

bool HttpClient::isRequestComplete() { return (_isRequestComplete); }
void HttpClient::setRequestComplete(bool state) { _isRequestComplete = state; }


/**********************************************************/
/******************* Client Interaction *******************/
/**********************************************************/

bool HttpClient::clientIsConnected() { return (this->_connectedToServer); }

void HttpClient::setClientInteraction(bool stat) {
  this->_connectedToServer = stat;
}
