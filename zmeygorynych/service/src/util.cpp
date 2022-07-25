//
// Created by Kurisu on 21/07/2022.
//
#include "util.h"

void failure(const std::string& msg, uint64_t lineNum){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cerr << fmt::format("[{}] {}: {} on line {}\n",
                             [&tm]() {
                                 std::stringstream ss;
                                 ss << std::put_time(&tm, "%H:%M:%S");
                                 return ss.str();
                             }(), RED+"Error"+RESET, msg, lineNum);
}

void debug(const std::string &msg) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cerr << fmt::format("[{}] {}: {}\n",
                             [&tm]() {
                                 std::stringstream ss;
                                 ss << std::put_time(&tm, "%H:%M:%S");
                                 return ss.str();
                             }(), "DEBUG", msg);

}

std::vector<std::pair<uint64_t, std::any>> enumerate(std::vector<std::any>& v) {
    std::vector<std::pair<uint64_t, std::any>> res;
    for (uint64_t i = 0; i < v.size(); i++){
        res.emplace_back(i, v);
    }
    return res;
}
