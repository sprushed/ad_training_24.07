//
// Created by Kurisu on 22/07/2022.
//

#include "Product.h"

Product::Product(Database *db, uint32_t id, uint32_t owner, const string &name, const string &description,
                 const string &secret, uint32_t cost) : Entity(db) {
    this->setOwner(owner);
    this->setId(id);
    this->setName(name);
    this->setDescription(description);
    this->setSecret(secret);
    this->setId(id);
    this->cost = cost;
    if (id == ENOID){
        writeToDB(false);
    } else {
        loadFromDB();
    }
}

void Product::writeToDB(bool exists){
    if (!exists)
        this->id = this->db->insertItem("products", {{"owner", this->owner}, {"name", this->name}, {"description", this->description}, {"secret", this->secret}, {"cost", this->cost}});
    else {
        this->db->updateItem("products", this->id, {{"owner", this->owner}, {"name", this->name}, {"description", this->description}, {"secret", this->secret}, {"cost", this->cost}});
    }
}

void Product::loadFromDB(){
    this->owner = this->db->getItem<uint32_t>("products", id, "owner");
    this->name = this->db->getItem<string>("products", id, "name");
    this->description = this->db->getItem<string>("products", id, "description");
    this->secret = this->db->getItem<string>("products", id, "secret");
    this->cost = this->db->getItem<uint32_t>("products", id, "cost");
}

uint32_t Product::getCost() const {
    return this->cost;
}
