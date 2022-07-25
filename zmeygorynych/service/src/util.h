//
// Created by Kurisu on 21/07/2022.
//

#ifndef SRC_UTIL_H
#define SRC_UTIL_H
#include <iostream>
#include <string>
#include <ctime>
#include <fmt/format.h>
#include <iomanip>
#include <vector>
#include <any>

#define RED std::string("\033[31m")
#define RESET std::string("\033[0m")

void failure(const std::string& msg, uint64_t lineNum);
void debug(const std::string& msg);
std::vector<std::pair<uint64_t, std::any>> enumerate(std::vector<std::any> v);

#endif //SRC_UTIL_H
