#include "CGI.hpp"
#include "HttpClient.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <_ctype.h>
#include <cstdlib>
#include <exception>
#include <string>
#include <cstring>

Location getLocationOfFastCGIextension(Path &requestFile,
                                       const servers_it &serverConf)
{

    if (!requestFile.has_extension())
    {
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

bool hasCGI(hfs::Path &path, const HttpRequest &req, const servers_it &serverConf)
{

    // list of cgi in the this location
    std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
    Location l = serverConf->at(location);
    strPair_t CGIList = l.getCGI();
    if (CGIList.size() == 0)
        return false;

    std::cout << "check for : " << path.extension() << "\n";
    strPair_it it = CGIList.find(path.extension());
    if (it == CGIList.end())
        return false;
    std::cout << "CGI You are looking for is : " << it->second << "\n";
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
                   bool appendHttp)
{

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
    if (header == "Authorization" ||
        header == "Proxy-Authorization" ||
        header == "WWW-Authenticate" ||
        header == "Proxy-Authenticate" ||
        header == "Authentication-Info" ||
        header == "X-Forwarded-User" ||
        header == "X-Auth-Token" ||
        header == "HTTP_ACCEPT")
        return (false);
    return true;
}

char **setEnv(const Path &reqResource, HttpClient &client)
{
    HttpRequest &req = client.req;
    HttpResponse &res = client.res;

    typedef std::map<std::string, std::string> HeaderType;
    HeaderType headers = client.req.getHeaders();
    char **env = new char *[headers.size() + 5];

    (void)res;
    (void)req;
    // std::cout << "Query string : " << reqResource.getQueryString() << "\n";
    // std::cout << "PATH : " << reqResource.c_str() << "\n";
    // std::cout << "EXT : " << reqResource.extension() << "\n";
    // std::cout << "req VErsion: " << req.getVersion() << "\n";
    // std::cout << "req method: " << req.getMethod() << "\n";
    // std::cout << "Header Proccessed: " << req.getHeaderProcessed() << "\n";
    // std::cout << "BODY : " << req.body << "\n";

    std::cout << "-- HEADER --\n";

    HeaderType::iterator it = headers.begin();
    int i = 0;
    for (; it != headers.end(); it++)
    {
        if (shouldPassToCGI(it->first))
        {
            env[i++] = formatHeader(it->first, it->second, true);
        }
    }
    env[i++] = formatHeader("SCRIPT_FILENAME", reqResource.c_str(), false);
    env[i++] = formatHeader("QUERY_STRING", reqResource.c_str(), false);
    env[i] = NULL;
    return (env);
    // exit(100);
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

void print_env(char **env)
{
    int i = 0;
    while (env[i])
    {
        std::cout << env[i] << "\n";
        i++;
    }
}

void executeCGIScriptAndGetResponse(const Path &reqResource,
                                    const servers_it &serverConf,
                                    HttpClient &client)
{
    HttpResponse &res = client.res;
    HttpRequest &req = client.req;
    (void)req;
    (void)serverConf;
    std::cout << "CGI Script not implemented yet" << std::endl;

    // HttpResponse res;

    // 2. execute the script and save the output in that file.
    std::cout << "!! Child procc PID :" << res.getProccessPID() << "\n";
    try
    {

        if (res.getProccessPID() == -1)
        {
            std::cout << "file to be execute = " << reqResource.c_str() << "\n";
            std::string fileName = generateUniqueName(client.getSocket());
            int fd = open(fileName.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
            if (fd == -1)
                throw std::exception();
            std::cout << "file create: " << fileName << "\n";
            // setEnv(reqResource, client); // temp
            // char **env = setEnv(reqResource, client);
            // print_env(env);
            // exit(100);
            int pid = fork();
            if (pid == -1)
                throw std::exception();
            if (pid == 0)
            {
                // setting the envirement
                char **env = setEnv(reqResource, client);
                // I NEED TO PROTECT THIS !!
                std::string location = req.findlocationOfUrl(req.getPath(), serverConf);
                Location CGILocation = serverConf->at(location);

                strPair_t CGIList = CGILocation.getCGI();

                std::string CGIScript = CGIList[reqResource.extension()];
                std::cout << "you CGI script is : " << CGIScript << "\n";
                dup2(fd, 1);

                execve(CGIScript.c_str(), NULL, env);
                res.setStatus(500);
                exit(13);
                std::cout << "I CAN NOT BABY!!!\n";
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
            // child is still there
            std::cout << "hey we still here!!\n";
            // client.setRespondComplete(false);
        }
        else if (p == -1 || (WIFEXITED(status) && WEXITSTATUS(status) == 13))
        {
            std::cout << "erro bay!\n";
            // exit(50);
            throw std::exception();
            //  error
        }
        else
        {
            res.setProccessPID(-1);
            std::cout << "sending your data\n";
            std::string f = res.getCGIFile().second;
            res.setFilename(http::filesystem::Path(f));
            res.setStatus(200);
            // client.setRespondComplete(true);
        }
        std::cout << "HANG ON PLEASE WORKING ON IT!!\n";
        // return (res);
    }
    catch (std::exception &e)
    {
        res.setStatus(500);
        if (res.getProccessPID() != -1)
            client.clean();
        res.setProccessPID(-1);
        std::cout << "error >> " << e.what() << "\n";
        // exit(50);
    }
}

Location getLocation(const HttpRequest &req, servers_it &serverConf)
{
    std::string requestedLocationName =
        req.findlocationOfUrl(req.getPath(), serverConf);
    Location location = serverConf->at(requestedLocationName);
    return (location);
}
