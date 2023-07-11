
#ifndef __HTTPREQUEST__H_
#define __HTTPREQUEST__H_
#include <sstream>
#include <map>
#include <iostream>
#include <algorithm>
#include "fileSystem.hpp"
#include "confAST.hpp"
/* some headers i found

    CONTENT_LENGTH – The length of the request body (as a string).
    CONTENT_TYPE – The MIME type of the request body.
    HTTP_ACCEPT – Acceptable content types for the response.
    HTTP_ACCEPT_ENCODING – Acceptable encodings for the response.
    HTTP_ACCEPT_LANGUAGE – Acceptable languages for the response.
    HTTP_HOST – The HTTP Host header sent by the client.
    HTTP_REFERER – The referring page, if any.
    HTTP_USER_AGENT – The client’s user-agent string.
    QUERY_STRING – The query string, as a single (unparsed) string.
    REMOTE_ADDR – The IP address of the client.
    REMOTE_HOST – The hostname of the client.
    REMOTE_USER – The user authenticated by the web server, if any.
    REQUEST_METHOD – A string such as "GET" or "POST".
    SERVER_NAME – The hostname of the server.
    SERVER_PORT – The port of the server (as a string).

 */
namespace hfs = http::filesystem;
class HttpRequest{
public:
  HttpRequest();
  HttpRequest(const HttpRequest &other);
  HttpRequest &operator=(const HttpRequest &other);
  ~HttpRequest();

  void parser(std::string rawData);

  void printRaw(){
    std::cout <<  this->_method << " - " << this->_path.c_str() << " - " << this->_version<< std::endl;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end() ; it++){
      std::cout << it->first << ":" << it->second << std::endl;
    }
  }
  std::string getMethod();
  http::filesystem::Path getPath()const;
  std::string getVersion();
  std::string findlocationOfUrl(const hfs::Path&	path,
					     const servers_it& conf)const;
  hfs::Path addRoot(const hfs::Path&    path,
				 const std::string&  location,
				 const servers_it&   conf)const; 
  hfs::Path getPathWRoot(const hfs::Path& path,
				      const servers_it& conf)const; 
  bool isRequestEnd();
private:
  std::string _method;
  http::filesystem::Path _path; // this a class which deal with proning staff of path
  std::string _version;
  std::map<std::string, std::string> headers;
  std::string _requestBuffer;
  size_t _chunkSize;
  bool  _requestEnded;

  void processRequestHeaders();
  void processRequestBody();
  void parseChunked();
  bool storeChunkToFile(std::string& chunk);



};



#endif //__HTTPREQUEST__H_


