#ifndef __UITLSFUNCTION_H__
#define __UITLSFUNCTION_H__

#include <catch2/catch_test_macros.hpp>
#include <curl/curl.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
namespace fs = std::filesystem;
bool createFile(std::string fileName, std::string content);
bool createDirectory(std::string dirPath);
bool removeDirectory(std::string directoryPath);
std::string ReadFileContent(const std::string& filepath) ;
bool removeFile(const std::string& filePath);
#endif