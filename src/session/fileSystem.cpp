#include "fileSystem.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>

using std::string;
using http::filesystem::Path;

http::filesystem::Path::Path(){
}
http::filesystem::Path::~Path(){
}
http::filesystem::Path::Path(string& path): _path(path){
}

http::filesystem::Path::Path(const Path& other) : _path(other._path) {
  // No need to call the assignment operator, directly initialize _path
}

Path& http::filesystem::Path::operator=(const Path& other) {
  if (this != &other) {
    _path = other._path;
  }
  return *this;
}


bool http::filesystem::Path::endswith(char del)const {
  if (_path.empty())
    return (false);
  
  return (_path.back() == del);
}

const char* http::filesystem::Path::c_str() const{
  return (_path.c_str());
}

string http::filesystem::Path::root_name(){ return ("");}
string http::filesystem::Path::root_directory(){
  if (_path[0] == '/')
    return (_path.substr(0, 0));
  return ("");
}

string http::filesystem::Path::root_path(){
   if (_path[0] == '/')
    return (_path.substr(0,0));
  return ("");
}

string http::filesystem::Path::relative_path(){
  if (_path[0] == '/')
    return (_path.substr(1));
  return ("");
}

string http::filesystem::Path::parent_path(){
  std::size_t found;

  found = _path.find_last_of("/");
  return (_path.substr(0, found));
}

string http::filesystem::Path::filename(){
  std::size_t found;

  found = _path.find_last_of("/");
  return (_path.substr(found + 1));
}

string http::filesystem::Path::stem(){
  std::size_t found, foundextension;
  found = _path.find_last_of("/");
  
  string tem = _path.substr(found + 1);
  foundextension = tem.find_last_of(".");
  return (tem.substr(0, foundextension));
}

string http::filesystem::Path::extension(){
  std::size_t found = _path.find_last_of(".");
  return (_path.substr(found));
}

bool http::filesystem::Path::has_root_path(){
  if (root_path().empty())
    return (false);
  return (true);
}

bool http::filesystem::Path::has_root_name(){
  if (root_name().empty())
    return (false);
  return (true);
}

bool http::filesystem::Path::has_root_directory() {
  if (root_directory().empty())
    return (false);
  return (true);
}

bool http::filesystem::Path::has_relative_path() {
  if (relative_path().empty())
    return (false);
  return (true);
}

bool http::filesystem::Path::has_parent_path() {
  if (parent_path().empty())
    return (false);
  return (true);
}

bool http::filesystem::Path::has_filename() {
  if (filename().empty())
    return (false);
  return (true);
}

bool http::filesystem::Path::has_stem() {
  if (stem().empty())
    return (false);
  return (true);
}

bool http::filesystem::Path::has_extension() {
  if (extension().empty())
    return (false);
  return (true);
}

bool      http::filesystem::isDirectory(const Path& path){
  
  struct stat s;
  if (lstat(path.c_str(), &s) == 0){
    if (S_ISDIR(s.st_mode))
      return (true);
  }else{
    std::cout << "error has been thrown or should be throwning" << std::endl;
  }
  return (false);
}

bool      http::filesystem::isRegular_file(const Path& path){
  struct stat s;
  if (lstat(path.c_str(), &s) == 0){
    if (S_ISREG(s.st_mode))
      return (true);
  }else{
    std::cout << "error has been thrown or should be throwning" << std::endl;
  }
  return (false);
}

bool      http::filesystem::isExests(const Path& path){
  struct stat s;
  return (stat (path.c_str(), &s) == 0);
}

bool      http::filesystem::uriEndsWithBackslash(const Path& uri){
  return (uri.endswith('/'));
}

bool      http::filesystem::isIndexExests(const Path& dirPath, const string& filename){
  DIR *dir;
  struct dirent *dp;

  if ((dir = opendir (dirPath.c_str())) == NULL) {
    perror ("Cannot open .");
    exit (1);
  }
  while ((dp = readdir (dir)) != NULL) {

    if (filename == dp->d_name){
      return (true);
    }
  }

  return (false);
}

file_type http::filesystem::type(const Path& path){
  struct stat s;
  file_type type;
  if ( lstat(path.c_str(), &s) == 0 ) {
    if ( S_ISDIR(s.st_mode) ) {
      // it's a directory
      type = DIRECTORY;
      return (type);
    } else if (S_ISREG(s.st_mode)) {
      // it's a file
      type = REGULAR;
      return (type);
    } else if (S_ISLNK(s.st_mode)) {
      // it's a symlink
      type = SYMLINK;
      return (type);
    } else {
      //something else
      type = UNKNOWN;
      return (type);
    }
  } else {
    //error
    std::cout << "error has been thrown or should be throwning" << std::endl;
  }
  type = NOT_FOUND;
  return (type);
}

void http::filesystem::Path::setPath(string path){
  _path = path;  
}


