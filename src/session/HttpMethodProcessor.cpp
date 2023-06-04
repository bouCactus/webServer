
#include "HttpMethodProcessor.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "fileSystem.hpp"
#include "confAST.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <ctime>

using		http::filesystem::Path;
namespace	hfs = http::filesystem;

bool resourceExists(Path& reqResource){
  return (hfs::isExests(reqResource));
}
HttpResponse createNotFoundResponse(servers_it& serverConf){
  HttpResponse	res;
  (void)serverConf;		// need to check about error page location : ask for getErrorPage() 
  res.defaultErrorResponse(404);
  return (res);
}
  
HttpResponse executeCGIScriptAndGetResponse(Path& reqResource, HttpRequest& , servers_it& serverConf){
  HttpResponse	res;
  (void)reqResource;
  (void)serverConf;
  return (res);
}

std::string getTimeGMT(){
  std::time_t	now = std::time(NULL);
  char		buffer[80];
  // Convert the time to a tm 
  std::tm*	tm  = std::gmtime(&now);
  std::strftime(buffer, 80, "%a, %d %b %Y %T GMT", tm);	// Format the time
  return (buffer);
}

std::string convertTimeToGMT(std::time_t time){
  struct tm	lt;
  char		buffer[80];
  std::string	strbuffer;
  localtime_r(&time, &lt);
  std::strftime(buffer, 80, "%a, %d %b %Y %T GMT", &lt);	// Format the time
  return (buffer);
}

std::string getType(Path& reqResource){	//just temp
  std::map<std::string, std::string>		types;
  std::map<std::string, std::string>::iterator	it;
  types["html"]	 = "text/html";
  types["htm"]	 = "text/html";
  types["shtml"] = "text/html";
  types["css"]	 = "text/css";
  types["jpb"]	 = "image/jpeg";
  types["js"]	 = "application/javascript";
  types["txt"]	 = "text/plain";
  types["mp4"]	 = "video/mp4";
  types["avi"]	 = "video/x-msvideo";
  it		 = types.find(reqResource.extension());

  if (it == types.end())
    return ("application/octet-stream") ;
  return (it->second);
}

HttpResponse createRegularFileResponse(Path& reqResource, const servers_it& serverConf){
  HttpResponse	res(200);
 

  (void)serverConf;		//  if (serverConf.getServerName())
    res.appendHeader("server", "webServer");
    res.appendHeader("Date", getTimeGMT());
   
    res.appendHeader("Content-Type", getType(reqResource));
    //    if (serverConf.getTimeOut() <= 0)
    res.appendHeader("Connection", "keep-alive");
    //    else
    //res.appendHeader("Connection", "done");
    res.appendHeader("Content-length", TO_STRING(hfs::fileSize(reqResource)));
    res.appendHeader("Last-Modified", convertTimeToGMT(hfs::getFileMTime(reqResource)));
    res.setFilename(reqResource);
    res.setBody("");
    std::cout << "request leaving createRegularFileResponse" << std::endl;
  return (res);
}

bool hasCGI(const Path& reqResource, const servers_it& serverConf){
  HttpResponse	res;
  (void)reqResource;
  (void)serverConf;
  return (false);
}

HttpResponse executeCGIScriptAndGetResponse(Path&		reqResource,
					    const HttpRequest	req,
					    const servers_it& serverConf){
   HttpResponse							res;
  (void)reqResource;
  (void)serverConf;
  (void)req;
  return (res);
}


hfs::Path getindex(const hfs::Path&	path,
		   const HttpRequest&	req,
		   const servers_it& serverConf){
  std::string	 indexes;
  std::string	 location = req.findlocationOfUrl(req.getPath(), serverConf);

  try{
    std::cout << "req.getPath()" << req.getPath().c_str() << std::endl;
    std::cout << "location>>" << "\"" << location << "\""<< std::endl;
    values_t     tmp	  = serverConf->at(location).getIndex();
    std::cout << *tmp.begin() << std::endl;
    for(values_it indexIt = tmp.begin(); indexIt != tmp.end(); indexIt++){
      std::cout << "indexes: " << *indexIt << std::endl;
      if (hfs::isIndexExests(path, *indexIt)){
	hfs::Path	tmpPath(path);
	tmpPath.appendFile(*indexIt);
	std::cout << "index exests" << std::endl;
	return (tmpPath);
      }
    }
    
  }catch(std::exception&){
    std::cout << "index location not found" << std::endl;
  }
  return (hfs::Path());
}


bool isAutoIndexEnabled(const HttpRequest& req, const servers_it& serverConf){
  std::string	location = req.findlocationOfUrl(req.getPath(), serverConf);
  try{
    if (serverConf->at(location).isAutoIndex())
      return (true);
  }catch(std::exception&){}
  return (false);
}
std::string listIndex(const HttpRequest& req, const servers_it& serverConf){
   DIR *dir;
  struct dirent *dp;
  std::stringstream st;
  std::string	    location = req.getPathWRoot(req.getPath(), serverConf).c_str();
  std::cout << ">>>>>>> " << location.c_str() << std::endl;
  if ((dir = opendir (location.c_str())) == NULL) {
    perror ("Cannot open .");
    exit (1);
  }
  while ((dp = readdir (dir)) != NULL) {
    std::string name = dp->d_name;
    if (dp->d_type == DT_DIR)
      name += "/";
    st <<"<a href=\""
       << req.getPath().c_str()
       << name
       << "\">"
       << dp->d_name
       << "</a>\n";
  }
  return (st.str());
}
HttpResponse getAutoIndex(const HttpRequest& req, const servers_it& serverConf){
  HttpResponse	    res(200);
  std::stringstream st;
  hfs::Path path = req.getPath();

  // req.getPathWRoot(req.getPath(),conf_S);

  (void)req;
  (void)serverConf;
  res.appendHeader("server", "webServer");
  res.appendHeader("Date", getTimeGMT()); 
  res.appendHeader("Content-Type", "text/html");
    //    if (serverConf.getTimeOut() <= 0)
  res.appendHeader("Connection", "keep-alive");
    //    else
    //res.appendHeader("Connection", "done");
  //++++++++++++++++++++
  st << "<!DOCTYPE html><head><title>"
     << "Index of "
     << req.getPath().c_str()
     << "</title></head><body><h1>"
     << req.getPath().c_str()
     << "</h1><hr><pre>"
     << listIndex(req, serverConf)
     << "</pre></body></html>";
    res.setBody(st.str());
    std::cout << res.getBody() << std::endl;
  res.appendHeader("Content-length", TO_STRING(res.getBodySize()));
  return (res);
}
//this function not full functional check carefully
HttpResponse createDirectoryResponse(hfs::Path&		reqResource,
				     const HttpRequest& req,
				     const servers_it& serverConf){
  HttpResponse	res;
  hfs::Path	indexPath;
  
  if (!reqResource.endswith('/')){
    res.defaultErrorResponse(301);
    return (res);
  }
  indexPath = getindex(reqResource, req, serverConf);
  std::cout << "go to the index: " << indexPath.c_str() << std::endl;
  if (indexPath.empty()){
    if (isAutoIndexEnabled(req, serverConf)){
      // return (getAutoIndex(req, serverConf));
      res = getAutoIndex(req, serverConf);
      std::cout << res.getStatus() << " " <<  res.getBody() << std::endl;
      return (res);
      
    }else{
    std::cout << "auto index is off" << std::endl;
    res.defaultErrorResponse(404);
    return (res);		//remember to create res inside defaultError
    }
  }else{
    if (hasCGI(indexPath, serverConf)) {
      return executeCGIScriptAndGetResponse(indexPath, req, serverConf);
    }
    return (createRegularFileResponse(indexPath, serverConf));
  }
}
 
// hfs::Path getPathRoot(hfs::Path path, servers_it& conf){
//   string tempPath	 = path.c_str();
//   string temp	 = path.c_str();
//   std::cout << "path	== >:" << tempPath << std::endl;
//   value_t root;
//   size_t pos		 = 0;
//   while (!tempPath.empty()){
//     try {
//       root		 = conf->at(tempPath).getRoot();
//       if (tempPath != "/")
// 	pos		 = tempPath.size();
//       break;
//     }catch (std::exception &) {
//       pos		 = tempPath.find_last_of("/");
//       tempPath.erase(pos);
//       if (tempPath.empty()){
// 	tempPath = "/";
// 	pos		 = 0;
//       }
//       std::cout << "affer earse:" <<  "\"" << tempPath << "\"" << std::endl;
//       std::cout << "No loaction match!" << std::endl;
//     }
//   }

//   path.setPath(temp.replace(0, pos, root));
//   std::cout << "getPathRoot:" << path.c_str() << std::endl;
//   return (path);
// }

HttpResponse HttpMethodProcessor::processGetRequest(HttpRequest&	req , servers_it& conf_S) {

  http::filesystem::Path	requestedResource = req.getPathWRoot(req.getPath(),conf_S);
 
  std::cout << "begin of processGet Path: "<< requestedResource.c_str()  << std::endl;
    if (!resourceExists(requestedResource)) {
        return createNotFoundResponse(conf_S);
    }

    else if (http::filesystem::isDirectory(requestedResource)) {
      std::cout << "-----inside directory--------" << std::endl;
      return createDirectoryResponse(requestedResource, req, conf_S);
    }

    else if (hasCGI(requestedResource, conf_S)) {
      return executeCGIScriptAndGetResponse(requestedResource, req, conf_S);
    }
    std::cout << "prepar to inter file response" << std::endl;
    return (createRegularFileResponse(requestedResource, conf_S));
}

HttpResponse HttpMethodProcessor::processPostRequest(HttpRequest&	req , servers_it& conf_S) {
  std::cout << "POST request not implemented "<< std::endl;
  HttpResponse	res;
  (void)req;
   (void)conf_S;
  return (res);			// for skiping error

}

HttpResponse HttpMethodProcessor::processDeleteRequest(HttpRequest&	req , servers_it& conf_S) {
  std::cout << "DELETE request not implemented "<< std::endl;

  (void)req;
   (void)conf_S;
  HttpResponse	res;
  
  return (res);			// for skiping error

}


