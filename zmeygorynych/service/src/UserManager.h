//
// Created by Kurisu on 21/07/2022.
//

#ifndef SRC_USERMANAGER_H
#define SRC_USERMANAGER_H

#include "Database.h"
class UserManager {
private:
    Database* db{};
    void init(bool privileged);
public:
    UserManager();
    explicit UserManager(bool privileged);
    bool exists(const string& username);
    User* authenticate(string username, string password);
    User* addUser(string username, string password);
    User* getUserById(uint32_t id);
    void updateUser(User* user);
    vector<Shop> getUserShops(User* user);
};

#endif //SRC_USERMANAGER_H
