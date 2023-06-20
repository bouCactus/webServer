
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
HttpResponse createErrorPageResponse(const servers_it& serverConf, const int statCode){
  HttpResponse	res;
  (void)serverConf;		// need to check about error page location : ask for getErrorPage() 
  res.defaultErrorResponse(statCode);
  return (res);
}
  
// HttpResponse executeCGIScriptAndGetResponse(Path& reqResource,
// 					    HttpRequest& req,
// 					    servers_it& serverConf){
//   HttpResponse	res;
//   (void)req;
//   (void)reqResource;
//   (void)serverConf;
//   return (res);
// }

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

HttpResponse executeCGIScriptAndGetResponse(const Path&		reqResource,
					    const HttpRequest&	req,
					    const servers_it& serverConf){
   HttpResponse	    res;
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
    res.defaultErrorResponse(403);
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
      return createErrorPageResponse(conf_S, 404);
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





HttpResponse delete_directory_recursive(const char *path,
					const servers_it& serverConf) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        // return defaultResponse(500); // Internal Server Error
	return createErrorPageResponse(serverConf, 500);
    }

    int has_write_access = access(path, W_OK);
    if (has_write_access != 0) {
        closedir(dir);
	return createErrorPageResponse(serverConf, 403);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string fileName = entry->d_name;
        if (fileName != "." && fileName != "..") {
            std::string filePath = std::string(path) + "/" + fileName;
            if (entry->d_type == DT_DIR) {
	      HttpResponse status = delete_directory_recursive(filePath.c_str(), serverConf);
	      if (status.getStatus() != 204) {
                    closedir(dir);
                    return status;
                }
            } else {
                if (std::remove(filePath.c_str()) != 0) {
                    closedir(dir);
                    // return defaultResponse(500); // Internal Server Error
		    return createErrorPageResponse(serverConf, 500);
                }
            }
        }
    }

    closedir(dir);

    if (std::remove(path) != 0) {
        // return defaultResponse(500); // Internal Server Error
	return createErrorPageResponse(serverConf, 500);
    }

    // return defaultResponse(204); // No Content
    return createErrorPageResponse(serverConf, 204);
}

HttpResponse delete_directory(const Path& path, const servers_it& serverConf) {
    // Ensure the path is not empty
  if (path.empty()) {
        // return defaultResponse(500); // Internal Server Error
    std::cout << "the path is empty: " << path.c_str() <<  std::endl;
	return createErrorPageResponse(serverConf, 500);
    }

  return delete_directory_recursive(path.c_str(), serverConf);
}

HttpResponse createDeleteDirectoryContent(hfs::Path& requestedResource,
					     const servers_it& serverConf){
  std::cout << "the requestedResource:" << requestedResource.c_str() << std::endl;
  return (delete_directory(requestedResource, serverConf));
}
HttpResponse createDeleteDirectoryResponse(hfs::Path&		reqResource,
				     const HttpRequest& req,
				     const servers_it& serverConf){
  HttpResponse	res;
  hfs::Path	indexPath;
  
  if (!reqResource.endswith('/')){
    // res.defaultErrorResponse(409);
    // return (res);
    return (createErrorPageResponse(serverConf, 409));
  }
  if (hasCGI(reqResource, serverConf)) {
    hfs::Path index =  getindex(reqResource, req, serverConf);
    if (index.empty()){
      // return (res.defaultErrorResponse(403));
      return createErrorPageResponse(serverConf, 403);
    }
    return executeCGIScriptAndGetResponse(indexPath, req, serverConf);
  }
  return (createDeleteDirectoryContent(reqResource, serverConf));
}
HttpResponse createDeleteRegularFileResponse(const hfs::Path& path,
					     const HttpRequest& req,
					     const servers_it& serverConf) {
  if (hasCGI(path, serverConf))
      return (executeCGIScriptAndGetResponse(path, req, serverConf));
  if (remove(path.c_str()) != 0) {
    // return defaultResponse(500); // Internal Server Error
    return createErrorPageResponse(serverConf, 500);
  }
  // return defaultResponse(204); // No Content
  return createErrorPageResponse(serverConf, 204);
}
HttpResponse HttpMethodProcessor::processDeleteRequest(HttpRequest&	req , servers_it& conf_S) {
  hfs::Path  requestedResource = req.getPathWRoot(req.getPath(), conf_S);
  if (!resourceExists(requestedResource)){
    return createErrorPageResponse(conf_S, 404);
  }
  else if (http::filesystem::isDirectory(requestedResource)) {
    std::cout << "-----inside directory--------" << std::endl;
    return createDeleteDirectoryResponse(requestedResource, req, conf_S);
  }
  else if (hasCGI(requestedResource, conf_S)) {
    return executeCGIScriptAndGetResponse(requestedResource, req, conf_S);
  }
  std::cout << "prepar to inter file response" << std::endl;
  return (createDeleteRegularFileResponse(requestedResource, req, conf_S));
}
// #-----------------------------------------------post--------------------------------------#//

HttpResponse createDirectoryPostResponse(hfs::Path&		reqResource,
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
    return createErrorPageResponse(serverConf, 403);
  }else{
    if (hasCGI(indexPath, serverConf)) {
      return executeCGIScriptAndGetResponse(indexPath, req, serverConf);
   }
    return (createErrorPageResponse(serverConf,403));
  }
}
bool isLocationAllowUpload(const HttpRequest& req,
			   const servers_it& serverConf){
   std::string	location = req.findlocationOfUrl(req.getPath(), serverConf);
  try{
    if (serverConf->at(location).isAllowed(POST))
      return (true);
  }catch(std::exception&){}
  return (false);
}
HttpResponse uploadFile(const hfs::Path& requestedResource,
			   const HttpRequest& req,
			   const servers_it& serverConf){
  // if request get the file or check body
  (void)requestedResource;
  (void)req;
  (void)serverConf;
  HttpResponse res;
  res.defaultErrorResponse(201);
  return (res);
}
HttpResponse HttpMethodProcessor::processPostRequest(HttpRequest&	req , servers_it& conf_S) {
  hfs::Path requestedResource = req.getPathWRoot(req.getPath(), conf_S);
  if (isLocationAllowUpload(req, conf_S)){
    return (uploadFile(requestedResource, req, conf_S));
  }else{
    if (!resourceExists(requestedResource)){
      return (createErrorPageResponse(conf_S, 404));
    }else if (hfs::isDirectory(requestedResource)){
      return (createDirectoryPostResponse(requestedResource, req, conf_S));
    }else if (hasCGI(requestedResource, conf_S)){
      return executeCGIScriptAndGetResponse (requestedResource, req, conf_S);
    }
    return (createErrorPageResponse(conf_S, 403));
  }
}

