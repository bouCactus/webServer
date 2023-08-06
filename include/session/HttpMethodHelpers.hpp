#include "fileSystem.hpp"
#include "HttpResponse.hpp"
#include "confAST.hpp"
#include <ctime>
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
#include "HttpClient.hpp"
#include "CGI.hpp"

using http::filesystem::Path;
namespace hfs = http::filesystem;

bool resourceExists(Path& reqResource);


void createErrorPageResponse(const servers_it& serverConf,
                                     const int statCode, HttpClient &client);

std::string getType(Path& reqResource);


void createRegularFileResponse(Path& reqResource,
                                       const servers_it& serverConf, HttpClient &client);

hfs::Path getindex(const hfs::Path& path, const HttpRequest& req,
                   const servers_it& serverConf);

bool isAutoIndexEnabled(const HttpRequest& req, const servers_it& serverConf);

std::string listIndex(const HttpRequest& req, const servers_it& serverConf);

void getAutoIndex(const servers_it& serverConf, HttpClient &client);

void createDirectoryResponse(hfs::Path& reqResource,
                                    HttpClient& client,
                                    servers_it& serverConf);

void delete_directory_recursive(const char* path,
                                        const servers_it& serverConf, HttpClient &client);

void delete_directory(const Path& path, const servers_it& serverConf, HttpClient &client) ;

void createDeleteDirectoryContent(hfs::Path& requestedResource,
                                          const servers_it& serverConf, HttpClient &client) ;

void createDeleteDirectoryResponse(hfs::Path& reqResource,
                                            HttpClient& client,
                                           const servers_it& serverConf);

void createDeleteRegularFileResponse(hfs::Path& path,
                                            HttpClient& client,
                                             const servers_it& serverConf);

void createDirectoryPostResponse(hfs::Path& reqResource,
                                         HttpClient &client,
                                         const servers_it& serverConf) ;

bool isLocationAllowUpload(const HttpRequest& req,
                           const servers_it& serverConf);

void uploadFile(servers_it& serverConf, HttpClient &client) ;
