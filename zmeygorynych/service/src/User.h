//
// Created by Kurisu on 21/07/2022.
//

#ifndef SRC_USER_H
#define SRC_USER_H
#include <string>
#include "Shop.h"
using namespace std;
class User {
private:
    string username;
    int32_t money;
    uint32_t id;
public:
    User(uint32_t id, const string &username = "", int32_t money = 100);
    string getUsername();
    int32_t getMoney();
    uint32_t getId();
    void updateMoney(int32_t delta);
};

#endif //SRC_USER_H
