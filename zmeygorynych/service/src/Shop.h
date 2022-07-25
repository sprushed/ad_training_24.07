//
// Created by Kurisu on 22/07/2022.
//

#ifndef SRC_SHOP_H
#define SRC_SHOP_H


class Entity;
class Product;
#include "Product.h"
#include "Entity.h"
#include <vector>

class Shop : public Entity {
public:
    explicit Shop(Database* db, uint32_t id, uint32_t owner = 0, const string &name = "", const string &description = "",
         const string &secret = "");
    Product createItem(const string& name, const string& description, const string& secret, uint32_t cost);
    vector<Product> getProducts();
    void writeToDB(bool exists);
    void loadFromDB();

};


#endif //SRC_SHOP_H
