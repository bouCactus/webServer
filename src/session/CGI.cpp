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
void createErrorPageResponse(const servers_it &serverConf, const int statCode,
                             HttpClient &client);
Location getLocationOfFastCGIextension(Path &requestFile,
                                       const servers_it &serverConf)
{
  if (!requestFile.has_extension())
  {
    LOG_THROW();
    throw std::exception();
  }
  std::string requestFileExtension = requestFile.extension();
  std::string locationName = "." + requestFileExtension + "$";
  Location location = serverConf->at(locationName);
  return (location);
}

bool hasCGI(hfs::Path &path, const HttpRequest &req,
            const servers_it &serverConf)
{
  std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
  Location l = serverConf->at(location);
  CGIMap_t CGIList = l.getCGI();

  if (CGIList.size() == 0)
    return false;

  CGIMap_it it = CGIList.find(path.extension());
  if (it == CGIList.end())
    return false;

  return true;
}

char *formatHeader(std::string const &header, std::string const &value,
                   bool appendHttp)
{

  int httpSize = 0;
  if (appendHttp)
    httpSize = 5;
  char *n = new char[httpSize + header.length() + value.length() + 2];
  if (appendHttp)
    std::strcpy(n, "HTTP_");
  typedef std::string::size_type size_str;
  size_str i = httpSize;
  size_str j = 0;
  for (; j < header.length(); j++)
  {
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

bool shouldPassToCGI(std::string const &header)
{
  if (header == "Authorization" || header == "Proxy-Authorization" ||
      header == "WWW-Authenticate" || header == "Proxy-Authenticate" ||
      header == "Authentication-Info" || header == "X-Forwarded-User" ||
      header == "X-Auth-Token" || header == "HTTP_ACCEPT")
    return (false);
  return true;
}

std::string getMimeType(HttpClient &client)
{
  (void)client;
  return "text/html";
}

char **setEnv(const Path &reqResource, HttpClient &client)
{
  HttpRequest &req = client.req;
  HttpResponse &res = client.res;
  (void)res;
  typedef std::map<std::string, std::string> HeaderType;
  HeaderType headers = client.req.getHeaders();
  char **env = new char *[headers.size() + 10];

  HeaderType::iterator it = headers.begin();
  int i = 0;
  for (; it != headers.end(); it++)
  {
    if (shouldPassToCGI(it->first))
    {
      env[i++] = formatHeader(it->first, it->second, true);
    }
  }
  env[i++] = formatHeader("SCRIPT_NAME", reqResource.c_str(), false);
  env[i++] = formatHeader("SCRIPT_FILENAME", reqResource.c_str(), false);
  env[i++] = formatHeader("QUERY_STRING", reqResource.getQueryString(), false);
  env[i++] = formatHeader("REQUEST_METHOD", req.getMethod(), false);
  env[i++] = formatHeader("SERVER_PROTOCOL", req.getVersion(), false);
  env[i++] = formatHeader("CONTENT_LENGTH", std::to_string(req.getContentLength()), false);
  string contentType = req.getHeaders()["Content-Type"];
  if (!contentType.empty())
    env[i++] = formatHeader("CONTENT_TYPE", contentType, false); 
  env[i++] = formatHeader("PATH_INFO", reqResource.c_str(), false);
  env[i++] = formatHeader("REDIRECT_STATUS", "200", false);
  env[i] = NULL;
  return (env);
}

std::string generateUniqueName(int socket)
{
  std::string fileName;
  std::stringstream s(fileName);
  time_t t = time(0);
  if (t == -1)
    throw std::exception();
  s << t;
  s >> fileName;
  return ("/tmp/CGI_temp_" + fileName + "_" + std::to_string(socket));
}

int getStatusCode(std::string &value)
{
  char *rest;
  std::string::size_type p = value.find_first_not_of(' ');
  if (p == std::string::npos)
  {
    throw std::exception();
  }
  value = value.substr(p);
  p = value.find_first_of(' ');
  if (p == std::string::npos)
    throw std::exception();
  std::string code = value.substr(0, p);
  int status = std::strtod(code.c_str(), &rest);
  if (status == 0)
    throw std::exception();
  return status;
}

void setHeaders(HttpClient &client, std::string &filename)
{
  HttpResponse &res = client.res;
  std::fstream out;
  std::ofstream tmp;
  std::string line;

  out.open(filename);
  std::string tmp_name(filename + "_tmp");
  tmp.open(tmp_name.c_str(), std::ios::out | std::ios::in | std::ios::trunc);
  int isStatusSet = 0;

  while (getline(out, line) && line != "\r")
  {
    std::string::size_type p = line.find_first_of(":");
    if (p == string::npos)
      throw std::exception();
    std::string key = line.substr(0, p);
    std::string value = line.substr(p + 1);
    p = value.find_last_of('\r');
    if (p != std::string::npos)
      value = value.substr(0, p);
    p = value.find_first_not_of(' ');
    if (p == string::npos)
      throw std::exception();
    value = value.substr(p);
    if (key.empty() || value.empty())
      throw std::exception();
    if (key == "Status")
    {
      res.setStatus(getStatusCode(value));
      isStatusSet = 1;
    }
    else
      res.appendHeader(key, value); 
  }
  if (line.empty() || !isStatusSet)
    throw std::exception();

  getline(out, line, '\0');
  tmp << line;
  res.appendHeader("Content-Length", std::to_string(line.size()));
  out.close();
  tmp.close();
  std::remove(filename.c_str());
  std::rename(tmp_name.c_str(), filename.c_str());
}

void executeCGIScriptAndGetResponse(const Path &reqResource,
                                    const servers_it &serverConf,
                                    HttpClient &client)
{
  HttpResponse &res = client.res;
  HttpRequest &req = client.req;

  try
  {
    std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
    Location CGILocation = serverConf->at(location);
    std::string reqMethod = req.getMethod();

    if ((reqMethod == "GET" && !CGILocation.isCGIAllowed(GET)) ||
        (reqMethod == "POST" && !CGILocation.isCGIAllowed(POST)) ||
        (reqMethod != "GET" && reqMethod != "POST"))
    {
      throw std::exception();
    }
    if (res.getProccessPID() == -1)
    {
      std::string fileName = generateUniqueName(client.getSocket());
      int fd = open(fileName.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);

      if (fd == -1)
        throw std::exception();
      int inFile = 0;
      if (req.getFormDataPart().size() != 0)
      {
        inFile = open((req.getFormDataPart())[0].filename.c_str(), O_RDWR);
      }
      client.setTimeOut(time(0));
      int pid = fork();
      if (pid == -1)
        throw std::exception();
      if (pid == 0)
      {
        char **env = setEnv(reqResource, client);
        char *arg[3];

        CGIMap_t CGIList = CGILocation.getCGI();
        std::string CGIScript = CGIList[reqResource.extension()];
        dup2(fd, 1);
        dup2(fd, 2);
        if (inFile)
          dup2(inFile, 0);
        arg[0] = (char *)CGIScript.c_str();
        arg[1] = (char *)reqResource.c_str();
        arg[2] = NULL;
        execve(CGIScript.c_str(), arg, env);
        exit(13);
      }
      else
      {
        res.setProccessPID(pid);
        res.setCGIFile(fd, fileName);
      }
    }
    int status;
    int p = waitpid(res.getProccessPID(), &status, WNOHANG);
    if (p == 0)
    {
      // if (!client.clientIsConnected())
      // {
      //   client.clean();
      //   res.setProccessPID(-1);
      // }
      char buff[10];
      if (read(client.getSocket(), &buff, 10) == 0)
      {
        client.clean();
        res.setProccessPID(-1);
        //std::cout << "client mach fhaloooo!\n" << buff << "\n";
      }
      else
      {
        if (time(0) - client.getTimeOut() > CGILocation.getCgiTimeOut())
          throw std::exception();
      }
    }
    else if (p == -1 || (WIFEXITED(status) && WEXITSTATUS(status) == 13))
      throw std::exception();
    else
    {
      res.setProccessPID(-1);
      std::string f = res.getCGIFile().second;
      setHeaders(client, f);
      res.setFilename(http::filesystem::Path(f));
    }
  }
  catch (std::exception &e)
  {
    createErrorPageResponse(serverConf, 500, client);
    //res.setStatus(500);
    if (res.getProccessPID() != -1)
      client.clean();
    res.setProccessPID(-1);
  }
}

Location getLocation(const HttpRequest &req, servers_it &serverConf)
{
  std::string requestedLocationName =
      req.findlocationOfUrl(req.getPath(), serverConf);
  Location location = serverConf->at(requestedLocationName);
  return (location);
}
