#ifndef __HTTPSTATUSCODES__H
#define __HTTPSTATUSCODES__H
#include <string>
#include <map>

class HttpStatusCodes{
public:
  enum HttpStatus
    {
      HTTP_CONTINUE                        = 100,
      HTTP_SWITCHING_PROTOCOLS             = 101,
      HTTP_OK                              = 200,
      HTTP_CREATED                         = 201,
      HTTP_ACCEPTED                        = 202,
      HTTP_NONAUTHORITATIVE                = 203,
      HTTP_NO_CONTENT                      = 204,
      HTTP_RESET_CONTENT                   = 205,
      HTTP_PARTIAL_CONTENT                 = 206,
      HTTP_MULTIPLE_CHOICES                = 300,
      HTTP_MOVED_PERMANENTLY               = 301,
      HTTP_FOUND                           = 302,
      HTTP_SEE_OTHER                       = 303,
      HTTP_NOT_MODIFIED                    = 304,
      HTTP_USEPROXY                        = 305,
      // UNUSED: 306
      HTTP_TEMPORARY_REDIRECT              = 307,
      HTTP_BAD_REQUEST                     = 400,
      HTTP_UNAUTHORIZED                    = 401,
      HTTP_PAYMENT_REQUIRED                = 402,
      HTTP_FORBIDDEN                       = 403,
      HTTP_NOT_FOUND                       = 404,
      HTTP_METHOD_NOT_ALLOWED              = 405,
      HTTP_NOT_ACCEPTABLE                  = 406,
      HTTP_PROXY_AUTHENTICATION_REQUIRED   = 407,
      HTTP_REQUEST_TIMEOUT                 = 408,
      HTTP_CONFLICT                        = 409,
      HTTP_GONE                            = 410,
      HTTP_LENGTH_REQUIRED                 = 411,
      HTTP_PRECONDITION_FAILED             = 412,
      HTTP_REQUESTENTITYTOOLARGE           = 413,
      HTTP_REQUESTURITOOLONG               = 414,
      HTTP_UNSUPPORTEDMEDIATYPE            = 415,
      HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
      HTTP_EXPECTATION_FAILED              = 417,
      HTTP_INTERNAL_SERVER_ERROR           = 500,
      HTTP_NOT_IMPLEMENTED                 = 501,
      HTTP_BAD_GATEWAY                     = 502,
      HTTP_SERVICE_UNAVAILABLE             = 503,
      HTTP_GATEWAY_TIMEOUT                 = 504,
      HTTP_VERSION_NOT_SUPPORTED           = 505
    };
  HttpStatusCodes() {
    statusCodes[HTTP_OK] = "OK";
    statusCodes[HTTP_MOVED_PERMANENTLY] = "Moved Permanently";
    statusCodes[400] = "Bad Request";
    // Add more status codes as needed
  }

  std::string getStatusMessage(int statusCode) {
    if (statusCodes.count(statusCode) > 0) {
      return statusCodes[statusCode];
    } else {
      return "Unknown Status Code";
    }
  }

private:
  std::map<int , std::string> statusCodes;
};
#endif // __HTTPSTATUSHANDLER__H
