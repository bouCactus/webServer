#include "HttpRequest.hpp"
#include "CGI.hpp"
#include "HttpClient.hpp"
#include "fileSystem.hpp"
#include "HttpTypes.hpp"
#include "HttpServer.hpp"
#include <fstream>
HttpRequest::HttpRequest(void) {
  std::cout << "Resquest: constructor not implemented yet" << std::endl;
  _chunkSize = -1;
  _contentLength = 0;
  _headersProcessed = false;
  _resouceCreatedSyccessfully = false;
};

HttpRequest::~HttpRequest(void) {
  std::cout << "Resquest: destructor not implemented yet" << std::endl;
  for (size_t i = 0; i < _parsed_parts.size(); ++i) {
    _parsed_parts[i].fileStream->close();
    delete _parsed_parts[i].fileStream;
  }
}

size_t HttpRequest::getContentLength() const { return _contentLength; }
std::map<std::string, std::string> &HttpRequest::getHeaders() {
  return headers;
}
std::vector<FormDataPart> HttpRequest::getFormDataPart() const {
  return _parsed_parts;
}

HttpRequest::HttpRequest(const HttpRequest &other) {
  //   std::cout << "Resquest: copy consturctor not implemented yet" <<
  //   std::endl;
  (void)other;
}
HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
  // std::cout << "Request: copy assignment not implemented yet" << std::endl;
  (void)other;
  return (*this);
}

void HttpRequest::setResourceCreatedSuccessfully(bool status) {
  _resouceCreatedSyccessfully = status;
}
bool HttpRequest::resourceIsCreatedSuccessfully() {
  return (_resouceCreatedSyccessfully);
}

std::string HttpRequest::getMethod() { return (_method); }
http::filesystem::Path HttpRequest::getPath() const { return (_path); }
std::string HttpRequest::getVersion() { return (_version); }

inline std::string &rtrim(std::string &s) {
  s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
  return s;
}

// trim from beginning of string (left)
inline std::string &ltrim(std::string &s) {
  s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
  return s;
}

// trim from both ends of string (right then left)
inline std::string &trim(std::string &s) { return ltrim(rtrim(s)); }

void HttpRequest::processRequestHeaders() {
  std::size_t endOfHeaders = _requestBuffer.find("\r\n\r\n");
  if (endOfHeaders != std::string::npos) {
    std::string headersData = _requestBuffer.substr(0, endOfHeaders);
    _requestBuffer =
        _requestBuffer.substr(endOfHeaders + 4, _requestBuffer.size());

    std::istringstream iss(headersData);
    std::string line;

    // Parse the request line
    std::getline(iss, line);
    std::istringstream lineStream(line);
    std::string path;
    lineStream >> _method >> path >> _version;
    _path.setPath(path);
    // Parse the headers
    while (std::getline(iss, line) && !line.empty()) {
      // Split each header line into key and value
      std::size_t colonPos = line.find(':');
      if (colonPos != std::string::npos) {
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        // Trim leading and trailing whitespaces from the key and value
        trim(value);
        trim(key);

        // Store the header in the headers map
        headers[key] = value;
      }
    }
    _headersProcessed = true;
  }
}

std::ofstream *createFile(const std::string &name) {
  std::ofstream *fileStream =
      new std::ofstream(name.c_str(), std::ios::binary | std::ios::app);
  if (!fileStream || !fileStream->is_open()) {
    std::cerr << "Failed to open file for storing chunked data." << std::endl;
    if (fileStream)
      delete fileStream; // Cleanup the dynamically allocated stream
                         // before returning NULL
    return NULL;
  }
  return fileStream;
}

std::string generateUniqueName() {

  time_t t = time(0);
  return (std::to_string(t));
};

std::string HttpRequest::getUploadPath(servers_it &serverConf){
  try{
    std::string pathLocation = this->findlocationOfUrl(this->_path,serverConf);
    Location location = serverConf->at(pathLocation);
    value_t uploadPath = location.getUploadPath();
    return (uploadPath);
  }catch(...){
    return "";
  }
}

std::string HttpRequest::getFileTypeFromContentType(std::string &contentType){
  
  std::map<std::string, std::string> types;
  std::map<std::string, std::string>::iterator it;
    types["text/html"] = "html";
    types["text/css"] = "css";
    types["image/jpeg"] = "jpg";
    types["application/javascript"] = "js";
    types["text/plain"] = "txt";
    types["video/mp4"] = "mp4";
    types["video/x-msvideo"] = "avi";
    it = types.find(contentType);
    if (it == types.end())
      return ("");
    return (it->second);
}

bool HttpRequest::prepareFileForPostRequest(servers_it &serverConf) {
  FormDataPart part;
  std::string &contentType = headers["Content-Type"];
  // Get the location for file upload
  std::string uploadPath = getUploadPath(serverConf);
  std::string fileNameGenerate = generateUniqueName();
  std::string fileExtension = getFileTypeFromContentType(contentType);

  part.name = fileNameGenerate;
  part.filename = uploadPath + "/uploadedFile" + fileNameGenerate + "." + fileExtension; 
  part.content_type = headers["Content-Type"];
  part.fileStream = createFile(part.filename);
  if (part.fileStream == NULL) {
    setResourceCreatedSuccessfully(false);
    return (false);
  }
  _parsed_parts.push_back(part); // Store the part for later use
  setResourceCreatedSuccessfully(true);
  return true;
}

bool HttpRequest::prepareFileForPostRequest(FormDataPart &part, servers_it &serverConf) {
  std::string uploadPath = getUploadPath(serverConf);
  if (!part.filename.empty()){
     hfs::Path tmpPath(part.filename);
     if (tmpPath.has_extension()){
        part.filename =  uploadPath + part.filename;
     }else{
        std::string fileExtension = getFileTypeFromContentType(part.content_type);
        part.filename =  uploadPath + part.name;
     }
  }else if(!part.name.empty()){
     part.filename = uploadPath + part.name ;
  }
  // TODO
  // check if file or just text
  part.fileStream = createFile(part.filename);
  if (!part.fileStream){
    setResourceCreatedSuccessfully(false);
    return (false);
  }
  setResourceCreatedSuccessfully(true);
  return (true);
}

void HttpRequest::closeFile() {
  if (!_parsed_parts.empty()) {
    FormDataPart part = _parsed_parts.back();
    part.fileStream->close();
  }
}

bool HttpRequest::storeChunkToFile(std::string &chunk, servers_it &serverConf) {
  if (_parsed_parts.empty()) {
    if (!prepareFileForPostRequest(serverConf)) {
      setResourceCreatedSuccessfully(false);
      return false;
    }
  }
  FormDataPart &part = _parsed_parts.back(); // Get the last file stream
  (*part.fileStream) << chunk;               // Write the chunk to the file
  setResourceCreatedSuccessfully(true);
  return true;
}

bool HttpRequest::parseChunkedEncoding(servers_it &serverConf) {
  while (!_requestBuffer.empty()) {
    if (_chunkSize == static_cast<size_t>(-1)) {
      size_t lineEnd = _requestBuffer.find("\r\n");
      if (lineEnd == std::string::npos)
        break;

      std::string chunkSizeStr = _requestBuffer.substr(0, lineEnd);

      char *endPtr;
      _chunkSize = std::strtoul(chunkSizeStr.c_str(), &endPtr, 16);
      if (endPtr !=
          chunkSizeStr.c_str() + lineEnd) // Check if the parsing was successful
        break;

      _requestBuffer.erase(0, lineEnd + 2);
    }

    if (_chunkSize == 0) {
      std::cout << "end of the chunks" << std::endl;
      return true;
    }

    if (_requestBuffer.length() >= _chunkSize + 2) {
      std::string chunkData = _requestBuffer.substr(0, _chunkSize);
      storeChunkToFile(chunkData, serverConf );
      _requestBuffer.erase(0, _chunkSize + 2);
      _chunkSize = static_cast<size_t>(-1);
    } else {
      break;
    }
  }
  return false;
}

FormDataPart parseMultipartFormData(std::string &headers) {
  FormDataPart part;
 
  std::stringstream ss(headers);
  std::string line;
  while (std::getline(ss, line)) {
    if (line.substr(0, 19) == "Content-Disposition") {
      size_t name_start = line.find("name=\"") + 6;
      size_t name_end = line.find("\"", name_start);
      part.name = line.substr(name_start, name_end - name_start);

      size_t filename_start = line.find("filename=\"", name_end) + 10;
      size_t filename_end = line.find("\"", filename_start);
      part.filename =
          line.substr(filename_start, filename_end - filename_start);
    } else if (line.substr(0, 13) == "Content-Type:") {
      size_t content_type_start = line.find(":") + 2;
      part.content_type = line.substr(content_type_start);
    }
  }
  return part;
}



void HttpRequest::parseMultipartFileContent(const std::string &content,
                                            size_t start, size_t end) {
  if (start >= content.size() || end > content.size() || start > end) {
    // Invalid positions, handle the error or return early
    return;
  }
  FormDataPart outputFile = _parsed_parts.back();
  // Write the content between the start and end positions to the file.
  for (size_t i = start; i < end; ++i) {
    outputFile.fileStream->put(content[i]);
  }
}

bool HttpRequest::parseBoundaryChunk(std::string &boundary, servers_it &serverConf) {
  const std::string boundaryPrefix = "--" + boundary + "\r\n";
  const std::string boundarySuffix = boundary + "--" + "\r\n";
  const size_t boundaryPrefixLength = boundaryPrefix.length();

  size_t nextBoundaryPos = _requestBuffer.find(boundaryPrefix);
  size_t endBoundaryPos = _requestBuffer.find(boundarySuffix);
  while (nextBoundaryPos != std::string::npos ||
         endBoundaryPos != std::string::npos) {
    if (nextBoundaryPos == 0) {
      size_t endOfHeaders = _requestBuffer.find("\r\n\r\n");
      if (endOfHeaders == std::string::npos)
        return false;

      std::string removeBoundary = _requestBuffer.substr(
          boundaryPrefixLength, endOfHeaders - boundaryPrefixLength);
      FormDataPart part = parseMultipartFormData(removeBoundary);
      _requestBuffer.erase(0, endOfHeaders + 4); // 4 = endOfHeaders.length();
      prepareFileForPostRequest(part, serverConf);
      _parsed_parts.push_back(part);
    } else if (nextBoundaryPos != std::string::npos) {
      parseMultipartFileContent(_requestBuffer, 0,
                                nextBoundaryPos - 2); // 2 for remove "\r\n"
      closeFile();
      _requestBuffer.erase(0, nextBoundaryPos);
    } else if (endBoundaryPos != std::string::npos) {
      parseMultipartFileContent(_requestBuffer, 0,
                                endBoundaryPos - 4); // 4 for remove "--\r\n"
      closeFile();
      _requestBuffer.clear();
      return true;
    }
    nextBoundaryPos = _requestBuffer.find(boundaryPrefix);
    endBoundaryPos = _requestBuffer.find(boundarySuffix);
  }
  if (!_requestBuffer.empty()) {
    parseMultipartFileContent(_requestBuffer, 0, _requestBuffer.length());
    _requestBuffer.clear();
  }
  return false;
}

bool HttpRequest::processRequestBodyContent(servers_it &serverConf) {
  const std::string &contentLength = headers["Content-Length"];
  const std::string &transferEncoding = headers["Transfer-Encoding"];
  const std::string &contentType = headers["Content-Type"];
  const std::string boundaryPrefix = "boundary=";

  if (transferEncoding == "chunked") {
    // Process chunked data
    std::cout << "|+|==============chunked==================|+|" << std::endl;
    return parseChunkedEncoding(serverConf);
  } else if (!contentLength.empty()) {
    _contentLength += _requestBuffer.size();
    size_t boundaryPos = contentType.find(boundaryPrefix);
    if (boundaryPos != std::string::npos) {
      // Process boundary
      // now if location has CGI whole body request goes to CGI
      http::filesystem::Path requestedResource =
          this->getPathWRoot(this->getPath(), serverConf);
      if (hasCGI(requestedResource, *this, serverConf)) {
        if (storeChunkToFile(_requestBuffer, serverConf)) {
          _requestBuffer.clear();
        }
      } else {
        std::string boundary =
            contentType.substr(boundaryPos + boundaryPrefix.size());
        std::cout << "|+|==============boundary==================|+|" << std::endl;
        return parseBoundaryChunk(boundary, serverConf);
      }
    }
  std::cout << "|+|==============content length==================|+|" << std::endl;
    // Process content with known length
    if (!_requestBuffer.empty() && storeChunkToFile(_requestBuffer, serverConf)) {
      _requestBuffer.clear();
    }
    // Do something when body overflow
    return (_contentLength == static_cast<size_t>(std::stoi(contentLength)));
  }
  // No content or unsupported encoding
  return true;
}

bool HttpRequest::characterNotAllowed(const std::string &path) {

  // Allowed Characters:
  // "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;="
  // ASCII Codes:        [ 33 ] [ 35 ] [ 36 ] [ 38---59 ] [ 61 ] [ 63---91 ] [
  // 93 ] [ 95 ] [ 97---122 ] [ 126 ]

  for (size_t i = 0; i < path.size(); i++) {
    if (path[i] != 33 and path[i] != 35 and path[i] != 36 and
        !(path[i] >= 38 && path[i] <= 59) and path[i] != 61 and
        !(path[i] >= 63 && path[i] <= 91) and path[i] != 93 and
        path[i] != 95 and !(path[i] >= 97 && path[i] <= 122) and
        path[i] != 126) {
      return (true);
    }
  }
  return (false);
}

int HttpRequest::checkRequestErrors(servers_it &serverConf) {
  const std::string &contentLength = headers["Content-Length"];
  const std::string &transferEncoding = headers["Transfer-Encoding"];
  const std::string &maxBodySize = serverConf->getMax();

  if (!transferEncoding.empty() && transferEncoding != "chunked") {
    return NOT_IMPLEMENTED;
  } else if (transferEncoding.empty() && contentLength.empty() &&
             this->getMethod() == "POST") {
    return BAD_REQUEST;
  } else if (characterNotAllowed(this->getPath().c_str())) {
    return BAD_REQUEST;
  } else if (strlen(this->getPath().c_str()) > MAX_CHARS_IN_PATH) {
    return REQUESTURITOOLONG;
  } else if (std::atoi(contentLength.c_str()) >
             std::atoi(maxBodySize.c_str()) * 1e6) {
    return REQUESTURITOOLONG;
  }
  return ACCURATE;
}

int HttpRequest::parseRequest(char *rawData, size_t bytesread,
                              servers_it &serverConf) {
  // _requestBuffer += rawData;
  _requestBuffer.append(rawData, bytesread);
  
  if (!_headersProcessed) {
    // Process request headers only if they haven't been parsed yet
    processRequestHeaders();
  }
  // // int errorFound = checkRequestErrors(serverConf);
  // // if (errorFound)
  // //     return (errorFound);
  // // Process the request body
  
  
  return processRequestBodyContent(serverConf);
}

std::vector<std::string> getAllDirectories(const std::string &path) {
  std::vector<std::string> directories;
  std::string::size_type pos = 0;

  // If the path starts with a slash, add it as the root directory
  if (!path.empty() && path[0] == '/') {
    directories.push_back("/");
    ++pos;
  }

  while (pos != std::string::npos) {
    std::string::size_type nextPos = path.find('/', pos);
    if (nextPos != std::string::npos) {
      directories.push_back(path.substr(pos, nextPos - pos));
      pos = nextPos + 1;
    } else {
      directories.push_back(path.substr(pos));
      pos = nextPos;
    }
  }

  return directories;
}

std::string HttpRequest::findlocationOfUrl(const hfs::Path &path,
                                           const servers_it &conf) const {
  const std::string separator = "/";
  const std::string rootDir = "/";

  std::string pathStr = path.c_str();
  size_t pos = pathStr.length(); // Start at the end of the string

  while (!pathStr.empty()) {
    try {
      conf->at(pathStr); // where check for location happened
      return (pathStr);
    } catch (...) {
      if (pos == 0) {
        break; // Reached the beginning of the string, break the loop
      }
      pos = pathStr.find_last_of(
          separator, pos - 1); // Find the last separator before 'pos'
      pathStr.resize(pos);     // Remove the last component from the path
    }
  }
  try {
    conf->at(rootDir);
    return (rootDir);
  } catch (...) {
    std::cout << "the exception of find location of url when return emtpy"
              << std::endl;
    return ("");
  }
}

hfs::Path HttpRequest::addRoot(const hfs::Path &path,
                               const std::string &location,
                               const servers_it &conf) const {
  const std::string separator = "/";
  const std::string rootDir = "/";
  size_t pos = (location == rootDir) ? 0 : location.size();
  value_t root;

  std::string pathStr = path.c_str();

  try {
    root = conf->at(location).getRoot();
  } catch (std::exception &) {
    std::cout << "page 500" << std::endl;
  }
  hfs::Path newPath(path);
  newPath.setPath(pathStr.replace(0, pos, root));

  return newPath;
}

hfs::Path HttpRequest::getPathWRoot(const hfs::Path &path,
                                    const servers_it &conf) const {
  // std::cout << path.c_str() << std::endl;
  value_t root = findlocationOfUrl(path, conf);
  // std::cout << "root: " << root << std::endl;
  return addRoot(path, root, conf);
}
