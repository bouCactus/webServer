#include "HttpRequest.hpp"
#include <fstream>
HttpRequest::HttpRequest(void){
  std::cout << "Resquest: constructor not implemented yet" << std::endl;
   _chunkSize = -1;
   _requestEnded = false;
};

HttpRequest::~HttpRequest(void){
  std::cout << "Resquest: destructor not implemented yet" << std::endl;
}

HttpRequest::HttpRequest(const HttpRequest &other){
  std::cout << "Resquest: copy consturctor not implemented yet" << std::endl;
  (void)other;
}
HttpRequest &HttpRequest::operator = (const HttpRequest &other){
  std::cout << "Request: copy assignment not implemented yet" << std::endl;
  (void)other;
  return (*this);

}


std::string HttpRequest::getMethod(){
  return (_method);
}
http::filesystem::Path HttpRequest::getPath()const {
  return (_path);
}
std::string HttpRequest::getVersion(){
  return (_version);
}

inline std::string& rtrim(std::string& s){
    s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s){
    s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trim(std::string& s){
    return ltrim(rtrim(s));
}

void HttpRequest::processRequestHeaders(){
  std::size_t endOfHeaders = _requestBuffer.find("\r\n\r\n");
  if (endOfHeaders != std::string::npos){
    std::string headersData = _requestBuffer.substr(0, endOfHeaders);
    _requestBuffer = _requestBuffer.substr(endOfHeaders, _requestBuffer.size());

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
	std::string whitespaces (" \t\f\v\n\r");
	// Trim leading and trailing whitespaces from the key and value
	trim(value);
	trim(key);

	// Store the header in the headers map
	headers[key] = value;
      }
    }
  }
}
bool HttpRequest::storeChunkToFile(std::string& chunk){
    std::ofstream file("chunked_data.txt", std::ios::binary | std::ios::app);
    if (!file) {
        std::cerr << "Failed to open file for storing chunked data." << std::endl;
        return false;
    }

    file.write(chunk.data(), chunk.size());
    file.close();
    return (true);
}
  


void HttpRequest::parseChunked() {
    while (!_requestBuffer.empty()) {
      if (_chunkSize == (size_t)-1) {
            size_t lineEnd = _requestBuffer.find("\r\n");
            if (lineEnd == std::string::npos)
                break;

            std::string chunkSizeStr = _requestBuffer.substr(0, lineEnd);

            try {
                _chunkSize = std::stoul(chunkSizeStr, nullptr, 16);
                _requestBuffer.erase(0, lineEnd + 2);
            } catch (...) {
                break;
            }
        }
        if (_chunkSize == 0){
            std::cout << "end of the chunks" << std::endl;
	    _requestEnded = true;
            break;
        }
        if (_requestBuffer.length() >= _chunkSize) {
            std::string chunkData = _requestBuffer.substr(0, _chunkSize);
            std::cout << chunkData;
	    storeChunkToFile(chunkData);
            _requestBuffer.erase(0, _chunkSize + 2);
            _chunkSize = -1;
        } else {
            break;
        }
    }
}

void HttpRequest::processRequestBody(){
  std::cout << "-----------------------body start-------------------" << std::endl;
  std::cout <<  _requestBuffer << std::endl;
  std::cout << "--------------------------------body end---------" << std::endl;

  std::string contentLength = headers["Content-Length"];
  std::string transferEncoding = headers["Transfer-Encoding"];
  if (!contentLength.empty()){
    std::cout << "Content lenght found" << std::endl;
    if (storeChunkToFile(_requestBuffer)){
      _requestBuffer.erase(0, _requestBuffer.size());
    }
  }
  else if (transferEncoding == "chunked"){
    parseChunked();
    std::cout << "chunked-----------------chunked" << std::endl;

  }else{
    std::cout << "chunked not found" << std::endl;
  }
}
void HttpRequest::parser(std::string rawData){
  _requestBuffer += rawData;

   if (_method.empty() || _path.empty() || _version.empty()) {
        // Request headers have already been parsed, process the request body
     processRequestHeaders();
   }
   std::cout << "processRequestHeaders()" << std::endl;
   processRequestBody();

   
     
}


std::string HttpRequest::findlocationOfUrl(const hfs::Path&	path,
					   const servers_it& conf)const {
  const std::string	separator = "/";
  const std::string	rootDir	  = "/";

  std::string	         pathStr = path.c_str();

  while (!pathStr.empty()) {
    try {
      conf->at(pathStr);
      return (pathStr);
    } catch (std::exception&) {
      size_t	pos = pathStr.find_last_of(separator);
      pathStr.erase(pos);
      if (pathStr.empty()) {
        pathStr	    = rootDir;
      }
    }
  }

  return "";
}

hfs::Path HttpRequest::addRoot(const hfs::Path&		path,
			       const std::string&	location,
			       const servers_it& conf)const {
  const std::string   separator = "/";
  const std::string   rootDir	  = "/";
  size_t	      pos = (location == rootDir) ? 0 : location.size();
  value_t	      root;

  std::string	pathStr = path.c_str();
 
  try{
    root = conf->at(location).getRoot();
  }catch(std::exception&){
    std::cout << "page 500" << std::endl;
  }
  hfs::Path	newPath(path);
  newPath.setPath(pathStr.replace(0, pos, root));

  return newPath;
}

hfs::Path HttpRequest::getPathWRoot(const hfs::Path& path, const servers_it& conf)const {
  value_t	root = findlocationOfUrl(path, conf);
  std::cout << "root: " << root << std::endl;
  return addRoot(path, root, conf);
}

bool HttpRequest::isRequestEnd(){
  return (_requestEnded);
}
