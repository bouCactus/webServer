#include "HttpRequest.hpp"

HttpRequest::HttpRequest(void){
  std::cout << "Resquest: constructor not implemented yet" << std::endl;
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
