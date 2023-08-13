#include "HttpMethodProcessor.hpp"

#include <cstddef>
#include <cstdio>
#include <dirent.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "confAST.hpp"
#include "confTypes.hpp"
#include "fileSystem.hpp"
#include "utilsFunction.hpp"
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <unistd.h>

#include "CGI.hpp"
#include "HttpMethodHelpers.hpp"

using http::filesystem::Path;
namespace hfs = http::filesystem;

void HttpMethodProcessor::processGetRequest(HttpClient &client,
                                            servers_it &conf_S) {
  HttpRequest &req = client.req;
  
  http::filesystem::Path requestedResource =
      req.getPathWRoot(req.getPath(), conf_S);
  // //std::cout << "-----------" << requestedResource.c_str() << std::endl;
  if (!resourceExists(requestedResource)) {
    // //std::cout << "generate error page for 404" << std::endl;
    createErrorPageResponse(conf_S, 404, client);
    return;
  }

  else if (http::filesystem::isDirectory(requestedResource)) {
    // //std::cout << "-----inside directory--------" << std::endl;
    createDirectoryResponse(requestedResource, client, conf_S);
    return;
  }

  else if (hasCGI(requestedResource, req, conf_S)) {
    executeCGIScriptAndGetResponse(requestedResource, conf_S, client);
    return;
  }
 
  // //std::cout << "prepar to inter file response" << std::endl;
  createRegularFileResponse(requestedResource, conf_S, client);
}

/*=============================================== delete request *
 * =================================*/

void HttpMethodProcessor::processDeleteRequest(HttpClient &client,
                                               servers_it &conf_S) {
  HttpRequest &req = client.req;

  hfs::Path requestedResource = req.getPathWRoot(req.getPath(), conf_S);
  if (!resourceExists(requestedResource)) {
    createErrorPageResponse(conf_S, 404, client);
    return;
  } else if (http::filesystem::isDirectory(requestedResource)) {
    // //std::cout << "-----inside directory--------" << std::endl;
    createDeleteDirectoryResponse(requestedResource, client, conf_S);
    return;
  } else if (hasCGI(requestedResource, req, conf_S)) {
    executeCGIScriptAndGetResponse(requestedResource, conf_S, client);
    return;
  }
  //std::cout << "prepar to inter file response" << std::endl;
  createDeleteRegularFileResponse(requestedResource, client, conf_S);
  return;
}
// #-----------------------------------------------post--------------------------------------#//

void HttpMethodProcessor::processPostRequest(HttpClient &client,
                                             servers_it &conf_S) {
  HttpRequest &req = client.req;

  // If a location supports FastCGI then the request should be redirected to
  // FastCGI.
  hfs::Path requestedResource = req.getPathWRoot(req.getPath(), conf_S);
  if (isLocationAllowUpload(req, conf_S)) {
    uploadFile(conf_S, client);
    if (hasCGI(requestedResource, req, conf_S)) {
      executeCGIScriptAndGetResponse(requestedResource, conf_S, client);
      return;
    }
    return;
  } else {
    if (!resourceExists(requestedResource)) {
      createErrorPageResponse(conf_S, 404, client);
      return;
    } else if (hfs::isDirectory(requestedResource)) {
      createDirectoryPostResponse(requestedResource, client, conf_S);
      return;
    } else if (hasCGI(requestedResource, req, conf_S)) {

      executeCGIScriptAndGetResponse(requestedResource, conf_S, client);
      return;
    }
    createErrorPageResponse(conf_S, 403, client);
      //exit(120);
    return;

  }
}
