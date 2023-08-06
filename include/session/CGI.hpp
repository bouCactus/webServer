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
#include "HttpClient.hpp"
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



Location getLocationOfFastCGIextension(Path& requestFile,
                                       const servers_it& serverConf);

bool hasCGI(hfs::Path& path, const HttpRequest& req, const servers_it& serverConf);

void setEnv(const Path &reqResource);

std::string generateUniqueName();

void executeCGIScriptAndGetResponse(const Path& reqResource,
                                            const servers_it& serverConf,
                                            HttpClient &client);

Location getLocation(const HttpRequest& req, servers_it& serverConf);