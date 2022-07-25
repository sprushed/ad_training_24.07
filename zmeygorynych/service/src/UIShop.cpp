//
// Created by Kurisu on 23/07/2022.
//
#include "UI.h"

void UI::UIShop::browseShop(Shop *shop, bool own) {
    cout << "Welcome to the shop " << shop->getName() << "!\n";
    vector<Product> prods = shop->getProducts();
    cout << "Here what this shop can deliver to you: \n";
    uint32_t i = 1;
    uint32_t maxId = prods.size();
    while (true) {
        try {
            for (uint32_t j = i; j < min(maxId + 1, i + 10); j++) {
                Product p = prods[j - 1];
                UI::drawBox({
                                    fmt::format(" {}. Name: {}", j, p.getName()),
                                    fmt::format(" Description: {}", p.getDescription()),
                                    fmt::format(" Cost: {}", p.getCost()),
                                    fmt::format(" Secret: {}", (own) ? p.getSecret() : "<redacted>")
                            }, 50);
            }
            UI::drawMenu({fmt::format("{:^48}", fmt::format("{}/{}", (i - 1) / 10 + 1,
                                                            ((maxId) ? floor((maxId - 1) / 10.) + 1 : 1)))}, 50);
            if (own)
                drawBox({"next - to go to next page", "prev - to go to previous page", "add - to add item",
                         "exit - to exit"}, 50);
            else
                drawBox({"next - to go to next page", "prev - to go to previous page", "buy - to buy item",
                         "exit - to exit"}, 50);
            string choice = UI::getInput("Your choice > ");
            if (choice == "next") {
                i += (i + 10 > maxId) ? 0 : 10;
            } else if (choice == "prev") {
                i -= (i < 10) ? 0 : 10;
            } else if (choice == "exit") {
                return;
            } else if (choice == "buy" && !own) {
                int id = UI::getNumber("Select item to buy > ");
                if (id > maxId) {
                    cout << "Invalid item" << endl;
                    continue;
                }
                Product p = prods[id - 1];
                if (UI::UIMain::loggedUser->getMoney() < p.getCost() &&
                    UI::UIMain::loggedUser->getId() != shop->getOwner()) {
                    cout << "Sorry, but you can't afford this knowledge..." << endl;
                    continue;
                } else {
                    User *owner = UI::priv->getUserById(shop->getOwner());
                    if (owner->getId() != UI::UIMain::loggedUser->getId()) {
                        UI::UIMain::loggedUser->updateMoney(-1 * p.getCost());
                        owner->updateMoney(p.getCost());
                        UI::priv->updateUser(UI::UIMain::loggedUser);
                        UI::priv->updateUser(owner);
                    }
                    UI::asciiPrinter("Thank you");
                    cout << "Here is how you get this package: " << p.getSecret() << endl;
                }
            } else if (choice == "add" && own) {
                string name = UI::getInput("Enter item name: ");
                if (find_if(prods.begin(), prods.end(), [&name](Product &obj) { return obj.getName() == name; }) !=
                    prods.end()) {
                    cout << "Sorry, you already have this item in your shop\n";
                    continue;
                }
                string description = UI::getInput("Enter description: ");
                string secret = UI::getInput("Enter directions for buyer: ");
                uint32_t cost = UI::getNumber("Enter cost of this item: ");
                prods.push_back(shop->createItem(name, description, secret, cost));
                maxId++;
                UI::asciiPrinter("Thank you");
            }
        } catch (const exception& e) {
            failure(e.what(), __LINE__);
            cout << e.what();
        }
    }
}
