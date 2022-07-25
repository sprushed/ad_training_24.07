//
// Created by Kurisu on 21/07/2022.
//

#ifndef SRC_UI_H
#define SRC_UI_H
#include <string>
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include "User.h"
#include "util.h"
#include "UserManager.h"
#include "asciiFont.h"
#include "unistd.h"
namespace UI {

    extern UserManager* priv;
    extern UserManager* unpriv;
    void init();
    std::string getInput(const std::string &prompt, bool isPassword=false);
    int getNumber(const std::string &prompt);
    void drawMenu(vector<string> list, int w = 30);
    void asciiPrinter(string msg);
    vector<long> fixAsciiMsg(string msg);
    [[noreturn]] void quit();

    namespace UILogin {
        User *loginUser();
        User *regUser();
        User *loginPage();
    };

    namespace UIMain {
        extern Database* db;
        extern User* loggedUser;
        void init(User *user);
        void mainMenu();
        void addShop();
        void listShops();
        void editShops();

        void userStats();

        void storeSecret();

    }

    namespace UIShop {
        void browseShop(Shop* shop, bool own = false);
        void createItem(Shop* shop);
    }

    void drawBox(vector<string> list, int w);
}


#endif //SRC_UI_H
