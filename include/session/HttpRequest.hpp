
#ifndef __HTTPREQUEST__H_
#define __HTTPREQUEST__H_
#include "confAST.hpp"
#include "fileSystem.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

namespace hfs = http::filesystem;

#define ACCURATE 0
#define CHECK_ALL 1
#define CHECK_BODY_LENGHT 2
#define NOT_IMPLEMENTED 501
#define BAD_REQUEST 400
#define REQUESTURITOOLONG 414
#define REQUESTENTITYTOOLARGE 413
#define MAX_CHARS_IN_PATH 2048

struct FormDataPart {
  std::string name;
  std::string filename;
  std::string content_type;
  std::string value;
  std::ofstream *fileStream;
  bool isFileOpen;
};

class HttpRequest {
public:
  std::string body;
  HttpRequest();
  HttpRequest(const HttpRequest &other);
  HttpRequest &operator=(const HttpRequest &other);
  ~HttpRequest();

  int parseRequest(char *rawData, size_t bytesread, servers_it &serverConf);

  // void printRaw(){
  //   //std::cout <<  this->_method << " - " << this->_path.c_str() << " - " <<
  //   this->_version<< std::endl; for (std::map<std::string,
  //   std::string>::iterator it = headers.begin(); it != headers.end() ; it++){
  //     //std::cout << it->first << ":" << it->second << std::endl;
  //   }
  // }
  void closeFile();
  std::string getMethod();
  http::filesystem::Path getPath() const;
  std::string getVersion();
  std::string findlocationOfUrl(const hfs::Path &path,
                                const servers_it &conf) const;
  hfs::Path addRoot(const hfs::Path &path, const std::string &location,
                    const servers_it &conf) const;
  hfs::Path getPathWRoot(const hfs::Path &path, const servers_it &conf) const;
  bool isRequestEnd();
  void setRequestEnd(bool state);
  void setResourceCreatedSuccessfully(bool status);
  bool resourceIsCreatedSuccessfully();
  bool getHeaderProcessed() { return (_headersProcessed); }
  std::map<std::string, std::string> &getHeaders();
  size_t getContentLength() const;
  std::vector<FormDataPart> getFormDataPart() const;
  std::string getUploadPath(servers_it &serverConf);

private:
  std::string _method;
  http::filesystem::Path
      _path; // this a class which deal with proning staff of path
  std::string _version;
  std::map<std::string, std::string> headers;
  std::string _requestBuffer;
  size_t _chunkSize;
  size_t _contentLength;
  bool _headersProcessed;
  bool _resouceCreatedSyccessfully;
  std::vector<FormDataPart> _parsed_parts;

  std::string getFileTypeFromContentType(std::string &contentType);
  void processRequestHeaders(); // it is not good idea (bool)
  bool processRequestBodyContent(servers_it &serverConf);
  bool parseChunkedEncoding(servers_it &serverConf);
  bool parseBoundaryChunk(std::string &boundary, servers_it &serverConf);
  bool prepareFileForPostRequest(servers_it &serverConf);
  bool prepareFileForPostRequest(FormDataPart &part, servers_it &serverConf);
  bool storeChunkToFile(std::string &chunk, servers_it &serverConf);
  void parseMultipartFileContent(const std::string &content, size_t start,
                                 size_t end);
  int checkRequestErrors(servers_it &serverConf);
  bool characterNotAllowed(const std::string &path);
};

#endif //__HTTPREQUEST__H_
