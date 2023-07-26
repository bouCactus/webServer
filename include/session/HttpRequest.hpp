
#ifndef __HTTPREQUEST__H_
#define __HTTPREQUEST__H_
#include <sstream>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "fileSystem.hpp"
#include "confAST.hpp"
#include <vector>
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

struct FormDataPart {
    string        name;
    string        filename;
    string        content_type;
    string        value;
    std::ofstream* fileStream;
    bool           isFileOpen;
};
class HttpRequest{
public:
  std::string body;
  HttpRequest();
  HttpRequest(const HttpRequest &other);
  HttpRequest &operator=(const HttpRequest &other);
  ~HttpRequest();

  bool parseRequest(const std::string rawData);

  void printRaw(){
    std::cout <<  this->_method << " - " << this->_path.c_str() << " - " << this->_version<< std::endl;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end() ; it++){
      std::cout << it->first << ":" << it->second << std::endl;
    }
  }
  void closeFile();
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
  void setRequestEnd(bool state);
  bool getHeaderProcessed(){
    return (_headersProcessed);
  }
private:
  std::string                       _method;
  http::filesystem::Path            _path; // this a class which deal with proning staff of path
  std::string                       _version;
  std::map<std::string, std::string> headers;
  std::string                       _requestBuffer;
  size_t                            _chunkSize;
  size_t                            _contentLength;
  bool                              _headersProcessed;
  std::vector<FormDataPart>         _parsed_parts;
  
  


  void processRequestHeaders(); // it is not good idea (bool)
  bool processRequestBodyContent();
  bool parseChunkedEncoding();
  bool parseBoundaryChunk( std::string& boundary);
  bool prepareFileForPostRequest();
  bool prepareFileForPostRequest(FormDataPart& part);
  bool storeChunkToFile(std::string& chunk);
  void parseMultipartFileContent(const std::string& content, size_t start, size_t end);
  


};



#endif //__HTTPREQUEST__H_


