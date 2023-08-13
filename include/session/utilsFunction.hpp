#ifndef __UTILSFUNCTION_H__
#define __UTILSFUNCTION_H__

#include <ctime>
#include <string>
#include "HttpClient.hpp"

std::string getTimeGMT();
std::string convertTimeToGMT(std::time_t time);
void serverLog(HttpClient &client);

#endif