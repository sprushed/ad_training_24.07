//
// Created by Kurisu on 21/07/2022.
//

#include "UserManager.h"
#include "Database.h"
#include "util.h"
void UserManager::init(bool privileged) {
    try {
        this->db = new Database(privileged);
    } catch (const exception& e) {
        failure("UserManager::init database initialization error", __LINE__);
        throw e;
    }
}

UserManager::UserManager() {
    this->init(false);
}

UserManager::UserManager(bool privileged) {
    this->init(privileged);
}

bool UserManager::exists(const string& username) {
    try {
        this->db->findId("credVault", {{"username", username}});
    } catch (const exception& e){
        failure(fmt::format("Requested non-existent user {}", username), __LINE__);
        failure(fmt::format("e.what(): {}", e.what()), __LINE__);
        return false;
    }
    return true;
}

User* UserManager::authenticate(string username, string password) {
    if (this->db->isPrivileged()) throw std::runtime_error("Internal error");
    if (!this->exists(username)) throw std::invalid_argument("User doesn't exist");
    uint32_t id;
    try {
        id = this->db->findId("credVault", {{"username", username}, {"password", password}});
    } catch (const exception& e){
        failure(fmt::format("Attempted wrong password {} for user {}", password, username), __LINE__);
        throw std::invalid_argument("Invalid password");
    }
    auto money = this->db->getItem<int32_t>("credVault", id, "money");
    return new User(id, username, money);
}

User * UserManager::addUser(string username, string password) {
    if (!this->db->isPrivileged()) throw std::runtime_error("Internal error");
    if (this->exists(username)) throw std::invalid_argument("User exists");
    uint32_t id = this->db->insertItem("credVault", {{"username", username}, {"password", password}, {"money", 100}});
    return new User(id, username);
}

User *UserManager::getUserById(uint32_t id) {
    if (!this->db->idExists("credVault", id)) throw std::invalid_argument("Id doesn't exist");
    return new User(
            id,
            this->db->getItem<string>("credVault", id, "username"),
            this->db->getItem<int32_t>("credVault", id, "money"));
}

void UserManager::updateUser(User* user) {
    if (!this->db->idExists("credVault", user->getId())) throw std::invalid_argument("Id doesn't exist");
    this->db->updateItem("credVault", user->getId(), {{"username", user->getUsername()}, {"money", user->getMoney()}});
}


vector<Shop> UserManager::getUserShops(User* user) {
    vector<uint32_t> shopIds = this->db->findIds("shops", {{"owner", std::to_string(user->getId())}});
    vector<Shop> res;
    for (auto &id: shopIds)
        res.emplace_back(this->db,
                         id,
                         user->getId(),
                         this->db->getItem<string>("shops", id, "name"),
                         this->db->getItem<string>("shops", id, "description"),
                         this->db->getItem<string>("shops", id, "secret"));
    return res;
}


