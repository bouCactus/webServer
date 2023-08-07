#include "HttpClient.hpp"
#include "HttpMethodProcessor.hpp"
#include "HttpMethodHelpers.hpp"
#include "HttpRequest.hpp"


bool resourceExists(Path& reqResource) {
    return (hfs::isExests(reqResource)); 
}


void createErrorPageResponse(const servers_it& serverConf,
                                     const int statCode, HttpClient &client) {
    HttpResponse &res = client.res;
    std::stringstream body;
    std::string statusMessage = res.status.getStatusMessage(statCode);
    std::string serverName = *(serverConf->getServerNames().begin());

    if (serverName.empty()) {serverName = serverConf->getHost();}
    res.defaultErrorResponse(statCode);
    res.appendHeader("Content-Type", "text/html");
    body
        << "<!DOCTYPE html><html><head><title>" << statCode << " "
        << statusMessage << "</title></head><body><h1><center>" << statCode
        << " " << statusMessage << "<hr>"
        << "</center></h1><p><center>"
        << serverName
        <<"</center></p></body></html>";
    res.setBody(body.str());
    res.appendHeader("Content-length", TO_STRING(res.getBodySize()));
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



void createRegularFileResponse(Path& reqResource,
                                       const servers_it& serverConf, HttpClient &client) {

    HttpResponse &res = client.res;
    res.setStatus(200);

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

void getAutoIndex(const servers_it& serverConf, HttpClient &client) {

    HttpResponse &res = client.res;
    HttpRequest &req = client.req;

    res.setStatus(200);
    res.setVersion("HTTP/1.1");// setting the version 
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
}

// this function not full functional check carefully
void createDirectoryResponse(hfs::Path& reqResource,
                                    HttpClient& client,
                                    servers_it& serverConf) {
    HttpRequest &req = client.req;
    HttpResponse &res = client.res;
    hfs::Path indexPath;

    if (!reqResource.endswith('/')) {
        res.defaultErrorResponse(301);
        return ;
    }
    if (client.res.getProccessPID() == -1)
    {
        indexPath = getindex(reqResource, req, serverConf);
        client.setIndexPath(indexPath);
    }
    std::cout << "go to the index: [" << indexPath.c_str() << "]" << std::endl;
    if (client.getIndexPath().empty()) {
        if (isAutoIndexEnabled(req, serverConf)) {
            // return (getAutoIndex(req, serverConf));
            std::cout << "<---------------autoIndexEnalbed---------------->>>"
                      << std::endl;
            getAutoIndex(serverConf, client);
            return ;

        } else {
            createErrorPageResponse(serverConf,403, client);
            return;
        }
    } else {
        if (hasCGI(client.getIndexPath(), req, serverConf)) {
            executeCGIScriptAndGetResponse(client.getIndexPath(), serverConf, client);
            return; 
        }
        createRegularFileResponse(client.getIndexPath(), serverConf, client);
        return ; 
    }
}




void delete_directory_recursive(const char* path,
                                        const servers_it& serverConf, HttpClient &client) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        // return defaultResponse(500); // Internal Server Error
        createErrorPageResponse(serverConf, 500, client);
        return;
    }

    int has_write_access = access(path, W_OK);
    if (has_write_access != 0) {
        closedir(dir);
        createErrorPageResponse(serverConf, 403, client);
        return ; 
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string fileName = entry->d_name;
        if (fileName != "." && fileName != "..") {
            std::string filePath = std::string(path) + "/" + fileName;
            if (entry->d_type == DT_DIR) {
                delete_directory_recursive(filePath.c_str(), serverConf, client);
                if (client.res.getStatus() != 204) {
                    closedir(dir);
                    //return client.res;
                }
            } else {
                if (std::remove(filePath.c_str()) != 0) {
                    closedir(dir);
                    // return defaultResponse(500); // Internal Server Error
                    createErrorPageResponse(serverConf, 500, client);
                    return ; 
                }
            }
        }
    }

    closedir(dir);

    if (std::remove(path) != 0) {
        // return defaultResponse(500); // Internal Server Error
        createErrorPageResponse(serverConf, 500, client);
        return ; 
    }

    // return defaultResponse(204); // No Content
    createErrorPageResponse(serverConf, 204, client);
    return ; 
}

void delete_directory(const Path& path, const servers_it& serverConf, HttpClient &client) {
    // Ensure the path is not empty
    if (path.empty()) {
        // return defaultResponse(500); // Internal Server Error
        std::cout << "the path is empty: " << path.c_str() << std::endl;
        createErrorPageResponse(serverConf, 500, client);
        return ; 
    }

    delete_directory_recursive(path.c_str(), serverConf, client);
}

void createDeleteDirectoryContent(hfs::Path& requestedResource,
                                          const servers_it& serverConf, HttpClient &client) {
    std::cout << "the requestedResource:" << requestedResource.c_str()
              << std::endl;
    delete_directory(requestedResource, serverConf, client);
}

void createDeleteDirectoryResponse(hfs::Path& reqResource,
                                            HttpClient& client,
                                           const servers_it& serverConf) {
    HttpRequest &req = client.req;
    hfs::Path indexPath;

    if (!reqResource.endswith('/')) {
        // res.defaultErrorResponse(409);
        // return (res);
        createErrorPageResponse(serverConf, 409, client);
        return ;
    }
    if (hasCGI(indexPath, req, serverConf)) {
        hfs::Path index = getindex(reqResource, req, serverConf);
        if (index.empty()) {
            // return (res.defaultErrorResponse(403));
            createErrorPageResponse(serverConf, 403, client);
            return; 
        }
        
        executeCGIScriptAndGetResponse(indexPath, serverConf, client);
        return ; 
    }
    createDeleteDirectoryContent(reqResource, serverConf, client);
}

void createDeleteRegularFileResponse(hfs::Path& path,
                                            HttpClient& client,
                                             const servers_it& serverConf) {
    HttpRequest &req = client.req;

    if (hasCGI(path, req, serverConf))
    {
        executeCGIScriptAndGetResponse(path, serverConf, client);
        return; 
    }
    if (remove(path.c_str()) != 0) {
        // return defaultResponse(500); // Internal Server Error
        createErrorPageResponse(serverConf, 500, client);
        return ; 
    }
    // return defaultResponse(204); // No Content
    createErrorPageResponse(serverConf, 204, client);
}



void createDirectoryPostResponse(hfs::Path& reqResource,
                                         HttpClient &client,
                                         const servers_it& serverConf) {
    HttpResponse &res = client.res;
    HttpRequest &req = client.req;

    hfs::Path indexPath;

    if (!reqResource.endswith('/')) {
        res.defaultErrorResponse(301);
        return;
    }
    indexPath = getindex(reqResource, req, serverConf);
    if (indexPath.empty()) {
        createErrorPageResponse(serverConf, 403, client);
        return ; 
    } else {
        if (hasCGI(indexPath, req, serverConf)) {
            executeCGIScriptAndGetResponse(indexPath, serverConf, client);
            return ; 
        }
        createErrorPageResponse(serverConf, 403, client);
        return; 
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

void uploadFile(servers_it& serverConf, HttpClient &client) {
    HttpRequest &req = client.req;
    HttpResponse &res = client.res;

    // if request get the file or check body
    if (req.resourceIsCreatedSuccessfully()) {
        res.defaultErrorResponse(201);
    } else {
        createErrorPageResponse(serverConf,400, client);
    }
}
