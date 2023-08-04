#include "HttpMethodProcessor.hpp"

#include <cstddef>
#include <dirent.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#include <ctime>
#include <string>
#include "confTypes.hpp"
#include "utilsFunction.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "confAST.hpp"
#include "fileSystem.hpp"
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctime>
#include <sstream>

using http::filesystem::Path;
namespace hfs = http::filesystem;

bool resourceExists(Path& reqResource) { return (hfs::isExests(reqResource)); }
void createErrorPageResponse(const servers_it& serverConf,
                                     const int statCode, HttpResponse &res) {
    (void)(serverConf);
    // i don't know how to use this getServerNames is it set of names, how
    //serverConf->getServerNames();
    std::string severName = "WebServer 0.1";
    std::stringstream body;
    std::string statusMessage = res.status.getStatusMessage(statCode);

    res.defaultErrorResponse(statCode);
    res.appendHeader("Content-Type", "text/html");
    body
        << "<!DOCTYPE html><html><head><title>" << statCode << " "
        << statusMessage << "</title></head><body><h1><center>" << statCode
        << " " << statusMessage << "<hr>"
        << "</center></h1><p><center>"
        << severName
        <<"</center></p></body></html>";
    res.setBody(body.str());
    res.appendHeader("Content-length", TO_STRING(res.getBodySize()));
    return (res);
}

std::string getType(Path& reqResource) {  // just temp
    std::map<std::string, std::string> types;
    std::map<std::string, std::string>::iterator it;
    types["html"] = "text/html";
    types["htm"] = "text/html";
    types["shtml"] = "text/html";
    types["css"] = "text/css";
    types["jpb"] = "image/jpeg";
    types["js"] = "application/javascript";
    types["txt"] = "text/plain";
    types["mp4"] = "video/mp4";
    types["avi"] = "video/x-msvideo";
    it = types.find(reqResource.extension());

    if (it == types.end()) return ("application/octet-stream");
    return (it->second);
}

HttpResponse createRegularFileResponse(Path& reqResource,
                                       const servers_it& serverConf) {
    HttpResponse res(200);

    (void)serverConf;  //  if (serverConf.getServerName())
    res.appendHeader("server", "webServer");
    res.appendHeader("Date", getTimeGMT());

    res.appendHeader("Content-Type", getType(reqResource));
    //    if (serverConf.getTimeOut() <= 0)
    res.appendHeader("Connection", "keep-alive");
    //    else
    // res.appendHeader("Connection", "done");
    res.appendHeader("Content-length", TO_STRING(hfs::fileSize(reqResource)));
    res.appendHeader("Last-Modified",
                     convertTimeToGMT(hfs::getFileMTime(reqResource)));
    res.setFilename(reqResource);
    res.setBody("");
    std::cout << "request leaving createRegularFileResponse" << std::endl;
    return (res);
}

Location getLocationOfFastCGIextension(Path& requestFile,
                                       const servers_it& serverConf) {

    if (!requestFile.has_extension()) {
        LOG_THROW();
        throw std::exception();
    }
    
    std::cout << "i need location that linked to : " << requestFile.c_str() << "\n";
    std::cout << "root_name : " << requestFile.c_str() << "\n";
    std::string requestFileExtension = requestFile.extension();
    std::string locationName = "." + requestFileExtension + "$";
    std::cout << "--- Looking for: " << locationName << "\n"; 
    Location location = serverConf->at(locationName);
    return (location);
}

bool hasCGI(hfs::Path& path, const HttpRequest& req, const servers_it& serverConf) {


    // list of cgi in the this location
    std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
    Location l = serverConf->at(location);
    strPair_t CGIList = l.getCGI();
    if (CGIList.size() == 0) return false;

    std::cout << "check for : " << path.extension() << "\n";
    strPair_it it = CGIList.find(path.extension());
    if (it == CGIList.end()) return false;
    std::cout << "CGI You are looking for is : " << it->second << "\n";
    return true;
    // strPair_it it =  CGIList.begin();
    // for(; it != CGIList.end(); it++){
    //     std::cout << it->first << " | " << it->second << "\n";
    // }
    // (void) req;
    // (void) serverConf;
    // return true;
    // std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
    // Location l = serverConf->at(location);
    // if (l.getCGI() == "") return false;
    // return true;
    // try {
    //     std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
    //     Location l = serverConf->at(location);
    //     std::cout << "checking CGI...!!\n";
    //     getLocationOfFastCGIextension(reqResource, serverConf);
    //     return (true);
    // } catch (...) {
    //     return (false);
    // }
}
extern char **environ;

void setEnv(const Path &reqResource) {
    setenv("SCRIPT_FILENAME", reqResource.c_str(), 1);
}

std::string generateUniqueName(){
    std::string fileName;
    std::stringstream s(fileName);
    s << time(0);
    s >> fileName;
    return ("CGI_temp_" + fileName);
}

HttpResponse executeCGIScriptAndGetResponse(const Path& reqResource,
                                            const HttpRequest& req,
                                            const servers_it& serverConf,
                                            HttpResponse &res) {
    std::cout << "CGI Script not implemented yet" << std::endl;
    
    //HttpResponse res;

    std::cout << "file to be execute = " << reqResource.c_str() << "\n";
    std::string fileName = generateUniqueName();
    int fd = open(fileName.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0777);
    std::cout << "file create: " << fileName <<"\n";
    // 2. execute the script and save the output in that file.
    
    int pid = fork();
    if (pid == 0)
    {
        // setting the envirement 
        setEnv(reqResource);
        char **env = environ;
        // I NEED TO PROTECT THIS !!
        std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
        Location CGILocation = serverConf->at(location);
        strPair_t CGIList = CGILocation.getCGI();
        std::string CGIScript = CGIList[reqResource.extension()];
        std::cout << "you CGI script is : " << CGIScript << "\n";
        dup2(fd, 1);
        execve(CGIScript.c_str(), NULL, env);
    }
    waitpid(pid, NULL, WNOHANG);
    res.setCGIFile(fd, fileName);
    res.setFilename(http::filesystem::Path(fileName));
    res.setStatus(200);
    return (res);
}

hfs::Path getindex(const hfs::Path& path, const HttpRequest& req,
                   const servers_it& serverConf) {
    std::string indexes;
    std::string location = req.findlocationOfUrl(req.getPath(), serverConf);

    try {
        values_t tmp = serverConf->at(location).getIndex();
        for (values_it indexIt = tmp.begin(); indexIt != tmp.end(); indexIt++) {
            if (hfs::isIndexExests(path, *indexIt)) {
                hfs::Path tmpPath(path);
                tmpPath.appendFile(*indexIt);
                return (tmpPath);
            }
        }

    } catch (std::exception&) {
        std::cout << "index location not found" << std::endl;
    }
    return (hfs::Path());
}

Location getLocation(const HttpRequest& req, servers_it& serverConf) {
    std::string requestedLocationName =
        req.findlocationOfUrl(req.getPath(), serverConf);
    Location location = serverConf->at(requestedLocationName);
    return (location);
}

bool isAutoIndexEnabled(const HttpRequest& req, const servers_it& serverConf) {
    try {
        std::string requestedLocationName =
            req.findlocationOfUrl(req.getPath(), serverConf);
        Location location = serverConf->at(requestedLocationName);
        return (location.isAutoIndex());
    } catch (...) {
        return (false);
    }
}
std::string listIndex(const HttpRequest& req, const servers_it& serverConf) {
    DIR* dir;
    struct dirent* dp;
    std::stringstream st;
    std::string location = req.getPathWRoot(req.getPath(), serverConf).c_str();
    if ((dir = opendir(location.c_str())) == NULL) {
        perror("Cannot open .");
        exit(1);
    }
    while ((dp = readdir(dir)) != NULL) {
        std::string name = dp->d_name;
        if (dp->d_type == DT_DIR) name += "/";
        st << "<a href=\"" << req.getPath().c_str() << name << "\">"
           << dp->d_name << "</a>\n";
    }
    return (st.str());
}
HttpResponse getAutoIndex(const HttpRequest& req,
                          const servers_it& serverConf) {
    HttpResponse res(200);
    std::stringstream responseBodyStream;
    hfs::Path requestedPath = req.getPath();

    res.appendHeader("server", "webServer");
    res.appendHeader("Date", getTimeGMT());
    res.appendHeader("Content-Type", "text/html");
    //    if (serverConf.getTimeOut() <= 0)
    res.appendHeader("Connection", "keep-alive");
    //    else
    // res.appendHeader("Connection", "done");
    //++++++++++++++++++++
    responseBodyStream << "<!DOCTYPE html><head><title>"
                       << "Index of " << requestedPath.c_str()
                       << "</title></head><body><h1>" << requestedPath.c_str()
                       << "</h1><hr><pre>" << listIndex(req, serverConf)
                       << "</pre></body></html>";
    res.setBody(responseBodyStream.str());
    std::cout << res.getBody() << std::endl;
    res.appendHeader("Content-length", TO_STRING(res.getBodySize()));
    return (res);
}
// this function not full functional check carefully
HttpResponse createDirectoryResponse(hfs::Path& reqResource,
                                     const HttpRequest& req,
                                     const servers_it& serverConf) {
    HttpResponse res;
    hfs::Path indexPath;

    if (!reqResource.endswith('/')) {
        res.defaultErrorResponse(301);
        return (res);
    }
    indexPath = getindex(reqResource, req, serverConf);
    std::cout << "go to the index: [" << indexPath.c_str() << "]" << std::endl;
    if (indexPath.empty()) {
        if (isAutoIndexEnabled(req, serverConf)) {
            // return (getAutoIndex(req, serverConf));
            std::cout << "<---------------autoIndexEnalbed---------------->>>"
                      << std::endl;
            res = getAutoIndex(req, serverConf);
            return (res);

        } else {
            return (createErrorPageResponse(serverConf,403));
        }
    } else {
        if (hasCGI(indexPath, req, serverConf)) {
            return executeCGIScriptAndGetResponse(indexPath, req, serverConf, res);
        }
        return (createRegularFileResponse(indexPath, serverConf));
    }
}

void HttpMethodProcessor::processGetRequest(HttpRequest& req,
                                                    servers_it& conf_S, HttpResponse &res) {
    http::filesystem::Path requestedResource =
        req.getPathWRoot(req.getPath(), conf_S);

    if (!resourceExists(requestedResource)) {
        std::cout << "generate error page for 404" << std::endl;
        res = createErrorPageResponse(conf_S, 404);
        return ; 
    }

    else if (http::filesystem::isDirectory(requestedResource)) {
        std::cout << "-----inside directory--------" << std::endl;
        res = createDirectoryResponse(requestedResource, req, conf_S);
        return; 
    }

    else if (hasCGI(requestedResource, req, conf_S)) {
        executeCGIScriptAndGetResponse(requestedResource, req, conf_S, res);
        return; 
    }
    std::cout << "prepar to inter file response" << std::endl;
    res = createRegularFileResponse(requestedResource, conf_S);
}
/*=============================================== delete request * =================================*/
HttpResponse delete_directory_recursive(const char* path,
                                        const servers_it& serverConf) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        // return defaultResponse(500); // Internal Server Error
        return createErrorPageResponse(serverConf, 500);
    }

    int has_write_access = access(path, W_OK);
    if (has_write_access != 0) {
        closedir(dir);
        return createErrorPageResponse(serverConf, 403);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string fileName = entry->d_name;
        if (fileName != "." && fileName != "..") {
            std::string filePath = std::string(path) + "/" + fileName;
            if (entry->d_type == DT_DIR) {
                HttpResponse status =
                    delete_directory_recursive(filePath.c_str(), serverConf);
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
        std::cout << "the path is empty: " << path.c_str() << std::endl;
        return createErrorPageResponse(serverConf, 500);
    }

    return delete_directory_recursive(path.c_str(), serverConf);
}

HttpResponse createDeleteDirectoryContent(hfs::Path& requestedResource,
                                          const servers_it& serverConf) {
    std::cout << "the requestedResource:" << requestedResource.c_str()
              << std::endl;
    return (delete_directory(requestedResource, serverConf));
}
HttpResponse createDeleteDirectoryResponse(hfs::Path& reqResource,
                                           const HttpRequest& req,
                                           const servers_it& serverConf) {
    HttpResponse res;
    hfs::Path indexPath;

    if (!reqResource.endswith('/')) {
        // res.defaultErrorResponse(409);
        // return (res);
        return (createErrorPageResponse(serverConf, 409));
    }
    if (hasCGI(indexPath, req, serverConf)) {
        hfs::Path index = getindex(reqResource, req, serverConf);
        if (index.empty()) {
            // return (res.defaultErrorResponse(403));
            return createErrorPageResponse(serverConf, 403);
        }
        return executeCGIScriptAndGetResponse(indexPath, req, serverConf, res);
    }
    return (createDeleteDirectoryContent(reqResource, serverConf));
}
HttpResponse createDeleteRegularFileResponse(hfs::Path& path,
                                             const HttpRequest& req,
                                             const servers_it& serverConf) {
    if (hasCGI(path, req, serverConf))
        return (executeCGIScriptAndGetResponse(path, req, serverConf, res));
    if (remove(path.c_str()) != 0) {
        // return defaultResponse(500); // Internal Server Error
        return createErrorPageResponse(serverConf, 500);
    }
    // return defaultResponse(204); // No Content
    return createErrorPageResponse(serverConf, 204);
}
void HttpMethodProcessor::processDeleteRequest(HttpRequest& req,
                                                       servers_it& conf_S, HttpResponse &res) {
    hfs::Path requestedResource = req.getPathWRoot(req.getPath(), conf_S);
    if (!resourceExists(requestedResource)) {
        res = createErrorPageResponse(conf_S, 404);
        return ; 
    } else if (http::filesystem::isDirectory(requestedResource)) {
        std::cout << "-----inside directory--------" << std::endl;
        res = createDeleteDirectoryResponse(requestedResource, req, conf_S);
        return ; 
    } else if (hasCGI(requestedResource, req, conf_S)) {
        res = executeCGIScriptAndGetResponse(requestedResource, req, conf_S, res);
        return ; 
    }
    std::cout << "prepar to inter file response" << std::endl;
    res = (createDeleteRegularFileResponse(requestedResource, req, conf_S));
    return ; 
}
// #-----------------------------------------------post--------------------------------------#//

HttpResponse createDirectoryPostResponse(hfs::Path& reqResource,
                                         const HttpRequest& req,
                                         const servers_it& serverConf) {
    HttpResponse res;
    hfs::Path indexPath;

    if (!reqResource.endswith('/')) {
        res.defaultErrorResponse(301);
        return (res);
    }
    indexPath = getindex(reqResource, req, serverConf);
    if (indexPath.empty()) {
        return createErrorPageResponse(serverConf, 403);
    } else {
        if (hasCGI(indexPath, req, serverConf)) {
            return executeCGIScriptAndGetResponse(indexPath, req, serverConf, res);
        }
        return (createErrorPageResponse(serverConf, 403));
    }
}
bool isLocationAllowUpload(const HttpRequest& req,
                           const servers_it& serverConf) {
    std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
    std::cout << "lcoation of post :" << location << std::endl;
    try {
        if (serverConf->at(location).isAllowed(POST)) return (true);
    } catch (std::exception&) {
    }
    return (false);
}
void uploadFile(servers_it& serverConf, HttpRequest& req, HttpResponse &res) {
    // if request get the file or check body
    if (req.resourceIsCreatedSuccessfully()) {
        res.defaultErrorResponse(201);
    } else {
        createErrorPageResponse(serverConf,400, res);
    }
}
void HttpMethodProcessor::processPostRequest(HttpRequest& req,
                                                     servers_it& conf_S, HttpResponse &res) {
    // If a location supports FastCGI then the request should be redirected to FastCGI.
    hfs::Path requestedResource = req.getPathWRoot(req.getPath(), conf_S);
    if (isLocationAllowUpload(req, conf_S)) {
        uploadFile(conf_S, req, res);
        return ;
    } else {
        if (!resourceExists(requestedResource)) {
            res = (createErrorPageResponse(conf_S, 404, res));
            return ; 
        } else if (hfs::isDirectory(requestedResource)) {
            res = (createDirectoryPostResponse(requestedResource, req, conf_S));
            return ; 
        } else if (hasCGI(requestedResource, req, conf_S)) {
            executeCGIScriptAndGetResponse(requestedResource, req,
                                                  conf_S, res);
            return;
        }
        res = (createErrorPageResponse(conf_S, 403, res));
        return ;
    }
}
