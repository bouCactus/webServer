#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include	<iostream>
#include <string>
#include <map>

using	std::string; 

    enum  file_type {

    NOT_FOUND  ,
    REGULAR  ,
    DIRECTORY  ,
    SYMLINK   ,
    UNKNOWN   
    /* implementation-defined */
};

namespace http{

  namespace filesystem{
    class Path{
    public:

      const char* c_str() const;
      string	root_name();
      string	root_directory();
      string	root_path();
      string	relative_path();
      string	parent_path();
      string	filename();
      string	stem();
      string	extension() const;
      


      bool	has_root_path();
      bool	has_root_name();
      bool	has_root_directory();
      bool	has_relative_path();
      bool	has_parent_path();
      bool	has_filename();
      bool	has_stem();
      bool	has_extension();
      bool      endswith(char del) const ;
      void	setPath(string path);
      void  setQuery(string query);
      bool      empty()const;
      void      appendFile(const string fileName);
      std::string getQueryString() const;
      Path();
      ~Path();
      Path(string& path);
      Path(const Path& other);
      Path&	operator = (const Path& other);
    private:
      string	_path;
      string  _queryString;
      std::map<std::string, std::string> query;
    };
    /*
      Constant 	| Meaning
      none 	        | indicates that the file status has not been evaluated yet, or an error occurred when evaluating it
      not_found 	| indicates that the file was not found (this is not considered an error)
      regular 	| a regular file
      directory 	| a directory
      unknown 	| the file exists but its type could not be determined 
    */

    bool	isDirectory(const Path& path);
    bool	isRegular_file(const Path& path);
    bool	isExests(const Path& path);
    bool	uriEndsWithBackslash(const Path& path);
    bool	isIndexExests(const Path& dir, const string& filename);
    file_type	type(const Path& path);
    size_t      fileSize(const Path& path);
    std::time_t getFileMTime(const Path& path);
    
  }
}
#endif
