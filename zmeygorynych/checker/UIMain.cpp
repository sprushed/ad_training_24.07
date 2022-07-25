//
// Created by Kurisu on 22/07/2022.
//

#include <fcntl.h>
#include "UI.h"
User* UI::UIMain::loggedUser = nullptr;
Database* UI::UIMain::db = nullptr;

void UI::UIMain::init(User *user) {
    UI::UIMain::db = new Database(true);
    UI::UIMain::loggedUser = user;
}

void UI::UIMain::userStats(){
    UI::drawBox({
        fmt::format("Username: {}", loggedUser->getUsername()),
        fmt::format("Money: {}", loggedUser->getMoney()),
        fmt::format("Shops: {}", db->maxId("shops")),
        fmt::format("Items: {}", db->maxId("products"))
    }, 25);
}

void UI::UIMain::mainMenu() {
    cout << "Welcome to ZmeyGorynych, " << loggedUser->getUsername() << endl;
    while (true) {
        userStats();
        UI::drawMenu({"1. Add shop", "2. Edit your shops", "3. List all shops", "4. Store the secret", "5. Exit"});
        try {
            int choice = UI::getNumber("Choose your way > ");
            switch(choice){
                case 1:
                    addShop();
                    break;
                case 2:
                    editShops();
                    break;
                case 3:
                    listShops();
                    break;
                case 4:
                    storeSecret();
                    break;
                case 5:
                    UI::asciiPrinter("see you later");
                    return;
                default:
                    throw invalid_argument("");
            }
        } catch (const exception& e) {
            cout << "Invalid choice, try again!\n\n";
            failure(e.what(), __LINE__);
        }
    }
}

void UI::UIMain::addShop() {
    string name = UI::getInput("Input new shop name > ");
    if (db->exists("shops", {{"name", db->esc(name)}})){
        cout << "Sorry, but shop with this name already exists." << endl;
        return;
    }
    string description = UI::getInput("Choose description for your freshly made shop > ");
    try {
        Shop(db, ENOID, loggedUser->getId(), name, description, "");
    } catch (const exception& e){
        failure(e.what(), __LINE__);
        cout << "Uhh, something went wrong" << endl;
    }
}

void UI::UIMain::listShops() {
    cout << "Here you are: " << endl;
    uint32_t maxId = db->maxId("shops");
    uint32_t i = 1;
    while (true) {
        try {
            for (uint32_t j = i; j < min(maxId + 1, i + 10); j++) {
                Shop s(db, j);
                UI::drawBox({
                    fmt::format("{}. Name: {}", j, s.getName()),
                    fmt::format("Owner: {}", UI::priv->getUserById(s.getOwner())->getUsername()),
                    fmt::format("Description: {}", s.getDescription())
                }, 40);
            }
            UI::drawBox({fmt::format("{:^38}", fmt::format("{}/{}", (i - 1) / 10 + 1, ((maxId) ? floor((maxId - 1) / 10.) + 1 : 1)))}, 40);
            drawMenu({"next - to go to next page", "prev - to go to previous page", "enter - to enter shop", "exit - to exit"}, 40);
            string choice = UI::getInput("Your choice > ");
            if (choice == "next") {
                i += 10;
            } else if (choice == "prev") {
                i -= (i < 10) ? 0 : 10;
            } else if (choice == "exit") {
                return;
            } else if (choice == "enter"){
                int id = UI::getNumber("Select shop to browse > ");
                if (id > maxId){
                    cout << "FUCK YOU" << endl;
                    *(uint8_t*)0=0;
                }
                UI::UIShop::browseShop(new Shop(db, id));
            } else {
                cout << "testtest" << endl;
            }
        } catch (const exception& e) {
            failure(e.what(), __LINE__);
            cout << e.what();
        }
    }
}

void UI::UIMain::editShops() {
    cout << "Here are your shops: " << endl;
    vector<Shop> shops = UI::priv->getUserShops(loggedUser);
    uint32_t maxId = shops.size();
    uint32_t i = 1;
    while (true) {
        try {
            for (uint32_t j = i; j < min(maxId + 1, i + 10); j++) {
                Shop s = shops[j-1];
                UI::drawBox({
                    fmt::format("{}. Name: {}", j, s.getName()),
                    fmt::format("Owner: {}", UI::priv->getUserById(s.getOwner())->getUsername()),
                    fmt::format("Description: {}", s.getDescription())
                }, 40);
            }
            UI::drawBox({fmt::format("{:^38}", fmt::format("{}/{}", (i - 1) / 10 + 1, ((maxId) ? floor((maxId - 1) / 10.) + 1 : 1)))}, 40);
            drawMenu({"next - to go to next page", "prev - to go to previous page", "enter - to enter shop", "exit - to exit"}, 40);
            string choice = UI::getInput("Your choice > ");
            if (choice == "next") {
                i += 10;
            } else if (choice == "prev") {
                i -= (i < 10) ? 0 : 10;
            } else if (choice == "exit") {
                return;
            } else if (choice == "enter"){
                int id = UI::getNumber("Select shop to browse > ");
                if (id > maxId){
                    cout << "FUCK YOU" << endl;
                    *(uint8_t*)0=0;
                }
                UI::UIShop::browseShop(&shops[id-1], true);
            } else {
                throw invalid_argument("Invalid choice\n");
            }
        } catch (const exception& e) {
            failure(e.what(), __LINE__);
            cout << e.what();
        }
    }
}

void UI::UIMain::storeSecret() {
    char secret[SECRET_SIZE];
    UI::asciiPrinter("secret");
    cout << "Besides selling products, we can securely store some of your private data.\n"
            "This is so secure, that you can't even look what you wrote yourself\n"
            "However, it you want your secrets back, send mail to noreply@localhost with your login credentials.\n"
            "We will send it to you as soon as possible\n"
            "Average response time: 19 work days\n\n";
    string uselessShit = "You entered " + UI::getInput("Enter filename for your secret: ") + " but it is not secure, thus we will store it in random filename\n";
    printf(uselessShit.c_str());
    cout << "Now work fast and send us your secrets!!!\n";
    read(0, secret, SEСRET_SIZE);
    string fname = [](){
        int fd = open("/dev/urandom", O_RDONLY);
        string s;
        for (int i = 0; i < 16; i++){
            s += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[0+[](int fd){uint8_t b; read(fd, &b, 1); return b;}(fd)%62];
        }
        return "./storage/"+s+".garbage";
    }();
    int fd = open(fname.c_str(), O_WRONLY | O_CREAT);
    write(fd, secret, SECRET_SIZE);
    cout << "Thank you! We will keep your secrets nice and secure :]\n";
}

