#include <iostream>
#include "UserManager.h"
#include "User.h"
#include "UI.h"
using namespace std;
int main() {
    UI::init();
    User* user = UI::UILogin::loginPage();
    UI::UIMain::init(user);
    UI::UIMain::mainMenu();
}
