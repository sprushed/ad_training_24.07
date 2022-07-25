//
// Created by Kurisu on 22/07/2022.
//
#include "UI.h"
User *UI::UILogin::loginUser() {
    string username;
    string password;

    username = UI::getInput("Your Username: ", false);
    password = UI::getInput("Password: ", true);

    try {
        return UI::unpriv->authenticate(username, password);
    } catch (const exception& e){
        cout << e.what() << endl;
        return nullptr;
    }
}
User *UI::UILogin::regUser() {
    string username;
    string password, password2;

    username = UI::getInput("Your brand new username: ", false);
    password = UI::getInput("Password: ", true);
    password2 = UI::getInput("One more time to be sure: ", true);
    if (password != password2) {
        cout << "Passwords do not match\n";
        return nullptr;
    }
    try {
        return UI::priv->addUser(username, password);
    } catch (const exception& e){
        cout << e.what() << endl;
        return nullptr;
    }
}

User *UI::UILogin::loginPage() {
    User* res = nullptr;
    while (!res) {
        UI::drawMenu({"1. Login", "2. Register", "3. Give Up..."});
        int choice;
        try {
            choice = UI::getNumber("Choose your way\n> ");
            switch (choice){
                case 1:
                    res = UI::UILogin::loginUser();
                    break;
                case 2:
                    res = UI::UILogin::regUser();
                    break;
                case 3:
                    UI::quit();
                    *(uint8_t*)nullptr=0;
                default:
                    throw invalid_argument("");
            }
        } catch (const exception& e){
            cout << "Invalid choice, try again!\n\n";
        }
    }
    return res;
}
