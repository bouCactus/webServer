#ifndef __CURLCONFIG_H__
#define __CURLCONFIG_H__

#include <catch2/catch_test_macros.hpp>
#include <curl/curl.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
struct ResponseData {
    long code;
    std::string body;
};
static size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
ResponseData PerformHttpGet(const std::string& url);

#endif