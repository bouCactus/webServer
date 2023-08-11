#include "CGI.hpp"
#include "HttpClient.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <_ctype.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

Location getLocationOfFastCGIextension(Path &requestFile,
                                       const servers_it &serverConf) {

  if (!requestFile.has_extension()) {
    LOG_THROW();
    throw std::exception();
  }

  // std::cout << "i need location that linked to : " << requestFile.c_str()
  // << "\n";
  // std::cout << "root_name : " << requestFile.c_str() << "\n";
  std::string requestFileExtension = requestFile.extension();
  std::string locationName = "." + requestFileExtension + "$";
  // std::cout << "--- Looking for: " << locationName << "\n";
  Location location = serverConf->at(locationName);
  return (location);
}

bool hasCGI(hfs::Path &path, const HttpRequest &req,
            const servers_it &serverConf) {
  // list of cgi in the this location
  std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
  Location l = serverConf->at(location);
  CGIMap_t CGIList = l.getCGI();
  // std::cout << "Looking in : " << l.getRoot() << "\n";
  // std::cout << "size of cgi > " << CGIList.size() << "\n";
  if (CGIList.size() == 0)
    return false;

  // std::cout << "check for : " << path.extension() << "\n";
  CGIMap_it it = CGIList.find(path.extension());
  if (it == CGIList.end())
    return false;

  //   std::cout << "Cheking fro CGI\n";
  //  std::cout << "CGI You are looking for is : " << it->second << "\n";
  return true;
}

extern char **environ;

/*
    NOTE : FROM RFC 3875 :
        4.1.18.  Protocol-Specific Meta-Variables.

        The HTTP header field name is converted to upper case, has all
        occurrences of "-" replaced with "_" and has "HTTP_" prepended to
        give the meta-variable name.  The header data can be presented as
        sent by the client, or can be rewritten in ways which do not change
        its semantics.  If multiple header fields with the same field-name
        are received then the server MUST rewrite them as a single value
        having the same semantics.  Similarly, a header field that spans
        multiple lines MUST be merged onto a single line.  The server MUST,
        if necessary, change the representation of the data (for example, the
        character set) to be appropriate for a CGI meta-variable.

        The server is not required to create meta-variables for all the
        header fields that it receives.  In particular, it SHOULD remove any
        header fields carrying authentication information, such as
        'Authorization'; or that are available to the script in other
        variables, such as 'Content-Length' and 'Content-Type'.  The server
        MAY remove header fields that relate solely to client-side
        communication issues, such as 'Connection'.

*/

char *formatHeader(std::string const &header, std::string const &value,
                   bool appendHttp) {

  int httpSize = 0;
  if (appendHttp)
    httpSize = 5;
  char *n = new char[httpSize + header.length() + value.length() + 2];
  if (appendHttp)
    std::strcpy(n, "HTTP_");

  // std::string newHeader = header;
  typedef std::string::size_type size_str;
  size_str i = httpSize;
  size_str j = 0;
  for (; j < header.length(); j++) {
    if (header[j] == '-')
      n[i++] = '_';
    else
      n[i++] = toupper(header[j]);
  }
  n[i++] = '=';
  j = 0;
  for (; j < value.length(); j++)
    n[i++] = value[j];
  n[i] = '\0';
  return (n);
}

bool shouldPassToCGI(std::string const &header) {
  if (header == "Authorization" || header == "Proxy-Authorization" ||
      header == "WWW-Authenticate" || header == "Proxy-Authenticate" ||
      header == "Authentication-Info" || header == "X-Forwarded-User" ||
      header == "X-Auth-Token" || header == "HTTP_ACCEPT")
    return (false);
  return true;
}

std::string getMimeType(HttpClient &client) {
  (void)client;
  return "text/html";
}

char **setEnv(const Path &reqResource, HttpClient &client) {
  HttpRequest &req = client.req;
  HttpResponse &res = client.res;
  (void)res;
  typedef std::map<std::string, std::string> HeaderType;
  HeaderType headers = client.req.getHeaders();
  char **env = new char *[headers.size() + 10];

  HeaderType::iterator it = headers.begin();
  int i = 0;
  for (; it != headers.end(); it++) {
    if (shouldPassToCGI(it->first)) {
      env[i++] = formatHeader(it->first, it->second, true);
    }
  }
  env[i++] = formatHeader("SCRIPT_NAME", reqResource.c_str(), false);
  env[i++] = formatHeader("SCRIPT_FILENAME", reqResource.c_str(), false);
  env[i++] = formatHeader("QUERY_STRING", reqResource.getQueryString(), false);
  env[i++] = formatHeader("REQUEST_METHOD", req.getMethod(), false);
  env[i++] = formatHeader("SERVER_PROTOCOL", req.getVersion(), false);
  env[i++] = formatHeader("CONTENT_LENGTH",
                          std::to_string(req.getContentLength()), false);
  env[i++] = formatHeader("CONTENT_TYPE", getMimeType(client),
                          false); // SHOULD COME FROM THE REQUEST
  env[i++] = formatHeader("PATH_INFO", reqResource.c_str(), false);
  env[i++] = formatHeader("REDIRECT_STATUS", "200", false);
  // HEADEAR NEEDED : REQUEST_METHOD
  env[i] = NULL;
  return (env);
}

std::string generateUniqueName(int socket) {
  std::string fileName;
  std::stringstream s(fileName);
  time_t t = time(0);
  if (t == -1)
    throw std::exception();
  s << t;
  s >> fileName;
  return ("/tmp/CGI_temp_" + fileName + "_" + std::to_string(socket));
}

void print_env(char **env) {
  int i = 0;
  while (env[i]) {
    // std::cout << env[i] << "\n";
    i++;
  }
}

int getStatusCode(std::string &value) {
  // std::cout << "checking : " << value << "\n";
  char *rest;
  std::string::size_type p = value.find_first_not_of(' ');
  if (p == std::string::npos) {
    throw std::exception();
  }
  value = value.substr(p);
  p = value.find_first_of(' ');
  if (p == std::string::npos)
    throw std::exception();

  // std::cout << "found in : "<< p << "\n";
  std::string code = value.substr(0, p);
  // std::cout << "code in str " << code << "\n";
  int status = std::strtod(code.c_str(), &rest);
  if (status == 0)
    throw std::exception();
  return status;
}

void setHeaders(HttpClient &client, std::string &filename) {
  // should remove the headers! and set them in res object. if no headers you
  // should throw 500 errror!
  HttpResponse &res = client.res;
  std::fstream out;
  std::ofstream tmp;

  (void)res;
  std::string line;
  out.open(filename);
  std::string tmp_name(filename + "_tmp");
  tmp.open(tmp_name.c_str(), std::ios::out | std::ios::in | std::ios::trunc);
  int isStatusSet = 0;

  while (getline(out, line) && line != "\r") {

    // std::cout << "header line : " << line <<  std::endl;
    std::string::size_type p = line.find_first_of(":");

    if (p == string::npos) {
      std::cout << ": not found\n";
      throw std::exception();
    }

    std::string key = line.substr(0, p);
    std::string value = line.substr(p + 1);
    p = value.find_last_of('\r');
    if (p != std::string::npos)
      value = value.substr(0, p);
    p = value.find_first_not_of(' ');
    if (p == string::npos) {
      std::cout << ": not found\n";
      throw std::exception();
    }
    value = value.substr(p);
    if (key.empty() || value.empty()) {
      std::cout << "empty val\n";
      throw std::exception();
    }
    if (key == "Status") {
      res.setStatus(getStatusCode(value));
      isStatusSet = 1;
    } else {
      std::cout << "key : [" << key << "] value [" << value << "]\n";
      res.appendHeader(key, value); // WHY THE HELL THIS NOT WORKING!!!!!!!!!!!
    }
    // getline(out, line);
  }
  if (line.empty() || !isStatusSet) {
    std::cout << "no status\n";
    throw std::exception();
  }
  // getline(out, line, '\r');
  // std::cout << line << "\n";
  // streampos oldpos = out.tellg();
  getline(out, line, '\0');
  // std::cout << line << "\n";
  tmp << line;

  res.appendHeader("Content-Length", std::to_string(line.size()));
  // if (!out.eof()) tmp << "\n";
  out.close();
  tmp.close();
  std::remove(filename.c_str());
  std::rename(tmp_name.c_str(), filename.c_str());

  // if (line.fin)
}

void printBody(const char *f) {
  std::cout << "FILE PATH : " << f << "\n";
  std::fstream out;
  out.open(f);
  std::string line;
  getline(out, line, '\0');
  std::cout << "BODY OF: " << line << "\n";
}

void executeCGIScriptAndGetResponse(const Path &reqResource,
                                    const servers_it &serverConf,
                                    HttpClient &client) {
  HttpResponse &res = client.res;
  HttpRequest &req = client.req;
  (void)req;
  (void)serverConf;

  // exit(118);
  //  std::cout << "req method: " << req.getMethod() << "\n";
  //  if (req.getMethod() == "POST")
  //  {
  //    std::cout << "GOT POST REQ! \n";
  //    std::cout << "BODY HERE " << req.body << "\n";
  //  }
  //  if (req.getFormDataPart().size() != 0)
  //  {
  //    std::cout << "FILE BODY!\n";
  //      printBody((req.getFormDataPart())[0].filename.c_str());

  // }
  try {
    std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
    Location CGILocation = serverConf->at(location);
    std::string reqMethod = req.getMethod();
    // check if allow or not!

    if ((reqMethod == "GET" && !CGILocation.isCGIAllowed(GET)) ||
        (reqMethod == "POST" && !CGILocation.isCGIAllowed(POST)) ||
        (reqMethod != "GET" && reqMethod != "POST")) {
      throw std::exception();
    }
    if (res.getProccessPID() == -1) {
      // std::cout << "file to be execute = " << reqResource.c_str() << "\n";
      std::string fileName = generateUniqueName(client.getSocket());
      int fd = open(fileName.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);

      if (fd == -1)
        throw std::exception();
      // this is where you can read dataƒ
      int inFile = 0;
      if (req.getFormDataPart().size() != 0) {
        std::cout << "FILE INPUT IS HEAR!!\n";
        inFile = open((req.getFormDataPart())[0].filename.c_str(), O_RDWR);
      }
      //(void) inFile;
      // std::cout << "file create: " << fileName << "\n";
      // setEnv(reqResource, client); // temp
      // char **env = setEnv(reqResource, client);
      // print_env(env);
      // exit(100);
      client.setTimeOut(time(0));
      int pid = fork();
      if (pid == -1)
        throw std::exception();
      if (pid == 0) {
        // setting the envirement
        char **env = setEnv(reqResource, client);
        char *arg[3];
        // I NEED TO PROTECT THIS !!

        CGIMap_t CGIList = CGILocation.getCGI();

        std::string CGIScript = CGIList[reqResource.extension()];
        // std::cout << "you CGI script is : " << CGIScript << "\n";

        dup2(fd, 1);
        dup2(fd, 2);
        if (inFile)
          dup2(inFile, 0);

        // char buf[1055];
        // int j = read(0, &buf, 1045);
        // buf[j] = '\0';
        // write(1, buf, 1045);
        // exit(1);
        arg[0] = (char *)CGIScript.c_str();
        arg[1] = (char *)reqResource.c_str();
        arg[2] = NULL;
        execve(CGIScript.c_str(), arg, env);
        // res.setStatus(500);
        exit(13);
        // std::cout << "I CAN NOT BABY!!!\n";
      } else {
        res.setProccessPID(pid);
        res.setCGIFile(fd, fileName);
      }
    }
    int status;
    int p = waitpid(res.getProccessPID(), &status, WNOHANG);
    if (p == 0) {
      // child is still there
      char buff[10];
      if (read(client.getSocket(), &buff, 10) == 0) {
        // buff[9] = '\0';
        client.clean();
        res.setProccessPID(-1);
        std::cout << "client mach fhaloooo!\n" << buff << "\n";
      } else {
        // std::cout << "WE are going to wait : " << CGILocation.getCgiTimeOut()
        // << "\n";
        if (time(0) - client.getTimeOut() > CGILocation.getCgiTimeOut()) {
          throw std::exception();
        }
      }
      // else
      // std::cout << "hey we still here!!\n";
      // client.setRespondComplete(false);
    } else if (p == -1 || (WIFEXITED(status) && WEXITSTATUS(status) == 13)) {
      // std::cout << "erro bay!\n";
      // exit(50);
      std::cout << "child exited quickly\n";
      throw std::exception();
      //  error
    } else {
      res.setProccessPID(-1);
      // std::cout << "sending your data\n";
      std::string f = res.getCGIFile().second;

      std::cout << "THIS IS THE EXT: " << reqResource.extension() << "\n";
      setHeaders(client, f);

      res.setFilename(http::filesystem::Path(f));
      std::cout << "done setting your data\n";
      // res.setStatus(200);
      //  client.setRespondComplete(true);
    }
    // std::cout << "HANG ON PLEASE WORKING ON IT!!\n";
    // return (res);
  } catch (std::exception &e) {
    res.setStatus(500);
    if (res.getProccessPID() != -1)
      client.clean();
    res.setProccessPID(-1);
    std::cout << "error >> " << e.what() << "\n";
    // exit(50);
  }
}

Location getLocation(const HttpRequest &req, servers_it &serverConf) {
  std::string requestedLocationName =
      req.findlocationOfUrl(req.getPath(), serverConf);
  Location location = serverConf->at(requestedLocationName);
  return (location);
}
