#include "utilsFunction.hpp"
#include "HttpClient.hpp"
std::string getTimeGMT() {
    std::time_t now = std::time(NULL);
    char buffer[80];
    // Convert the time to a tm
    std::tm* tm = std::gmtime(&now);
    std::strftime(buffer, 80, "%a, %d %b %Y %T GMT", tm);  // Format the time
    return (buffer);
}

std::string convertTimeToGMT(std::time_t time) {
    struct tm lt;
    char buffer[80];
    std::string strbuffer;
    localtime_r(&time, &lt);
    std::strftime(buffer, 80, "%a, %d %b %Y %T GMT", &lt);  // Format the time
    return (buffer);
}

void serverLog(HttpClient &client){
    
    std::map<std::string, std::string> &headers = client.req.getHeaders();
    std::cout << "[" << headers["Host"] << " \""
    << client.req.getMethod() << " " 
    << client.req.getPath().c_str() << " "
    << client.req.getVersion() << " \" " 
    << client.res.getStatus() << " "
    << client.res.getBodySize() << "]" << std::endl;
}
