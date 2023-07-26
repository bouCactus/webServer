#ifndef __HTTPRESPONSE__H_
#define __HTTPRESPONSE__H_

#include <netinet/in.h>
#include <unistd.h>

#include <map>

#include "HttpRequest.hpp"
#include "HttpStatusCodes.hpp"
typedef std::map<std::string, std::string> Smap_t;

#define TO_STRING(x)          \
    ({                        \
        std::stringstream ss; \
        ss << (x);            \
        ss.str();             \
    })

class HttpResponse {
   public:
    HttpStatusCodes status;

    HttpResponse(void);
    HttpResponse(int statusCode);
    HttpResponse(const HttpResponse &other);
    HttpResponse &operator=(const HttpResponse &other);
    ~HttpResponse(void);
    void appendHeader(std::string name, std::string value);
    void setVersion(const std::string version);
    void setStatus(const int status);
    void setBody(std::string body);
    void setFilename(http::filesystem::Path path);
    http::filesystem::Path getFilename();
    void sendFile(const std::string path);
    void setDate(const std::string date);
    int getBodySize();
    std::string getBody();
    int getStatus();
    std::string getVersion();
    std::string getHeaders();
    void defaultErrorResponse(int status);
    void writeHeader(int satuts, Smap_t &header);

   private:
    Smap_t _headers;
    http::filesystem::Path _fileName;
    int _status;
    std::string _version;
    std::string _date;
    std::string _body;
};
#endif  // __THHPRESPONSE__H_

// implementation
