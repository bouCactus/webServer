#include <iostream>
#include "../include/session/fileSystem.hpp"

class req{
    public:
    http::filesystem::Path getPath();
    private:
    http::filesystem::Path _path;
};   
http::filesystem::Path req::getPath(){
    _path.setPath("/home/to/include/file.text");
    return (_path);
}
int main(){
req q;
http::filesystem::Path p = q.getPath();
std::cout << p.getPath() << std::endl;
 return (0);
}