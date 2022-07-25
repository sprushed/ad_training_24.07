//
// Created by Kurisu on 21/07/2022.
//

#include "User.h"
#include "Shop.h"
User::User(uint32_t id, const string &username, int32_t money) {
    this->username = username;
    this->money = money;
    this->id = id;
}

string User::getUsername() {
    return this->username;
}

int32_t User::getMoney() {
    return this->money;
}

uint32_t User::getId() {
    return this->id;
}

void User::updateMoney(int32_t delta) {
    this->money += delta;
}
