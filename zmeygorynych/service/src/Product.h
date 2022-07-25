//
// Created by Kurisu on 22/07/2022.
//

#ifndef SRC_PRODUCT_H
#define SRC_PRODUCT_H

class Entity;
#include "Database.h"
#include "Entity.h"
#include <vector>

class Product :  public Entity {
private:
    uint32_t cost;
public:
    Product(Database *db, uint32_t id, uint32_t owner, const string &name, const string &description,
            const string &secret, uint32_t cost);

    void loadFromDB();
    uint32_t getCost() const;
    void writeToDB(bool exists);
};


#endif //SRC_PRODUCT_H
