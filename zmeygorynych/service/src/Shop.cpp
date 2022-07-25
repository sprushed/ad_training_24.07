//
// Created by Kurisu on 22/07/2022.
//

#include "Shop.h"



Shop::Shop(Database *db, uint32_t id, uint32_t owner, const string &name, const string &description,
           const string &secret) : Entity(db) {
    this->setOwner(owner);
    this->setName(name);
    this->setDescription(description);
    this->setSecret(secret);
    this->setId(id);
    if (id == ENOID){
        this->writeToDB(false);
    } else {
        this->loadFromDB();
    }
}



vector<Product> Shop::getProducts() {
    vector<uint32_t> prodIds = this->db->findIds("products", {{"owner", std::to_string(this->id)}});
    vector<Product> res;
    for (auto& id : prodIds) res.emplace_back(db,
                              this->db->getItem<uint32_t>("products", id, "id"),
                              this->id,
                              this->db->getItem<string>("products", id, "name"),
                              this->db->getItem<string>("products", id, "description"),
                              this->db->getItem<string>("products", id, "secret"),
                              this->db->getItem<uint32_t>("products", id, "cost"));
    return res;
}

void Shop::writeToDB(bool exists){
    if (!exists)
        this->id = this->db->insertItem("shops", {{"owner", this->owner}, {"name", this->name}, {"description", this->description}, {"secret", this->secret}});
    else {
        this->db->updateItem("shops", this->id, {{"owner", this->owner}, {"name", this->name}, {"description", this->description}, {"secret", this->secret}});
    }
}

void Shop::loadFromDB(){
    this->owner = this->db->getItem<uint32_t>("shops", id, "owner");
    this->name = this->db->getItem<string>("shops", id, "name");
    this->description = this->db->getItem<string>("shops", id, "description");
    this->secret = this->db->getItem<string>("shops", id, "secret");
}

Product Shop::createItem(const string& name, const string& description, const string& secret, uint32_t cost){
    if (this->db->exists("products", {{"owner", to_string(this->getId())}, {"name", name}})) throw invalid_argument("Item exists");
    return {this->db, ENOID, this->getId(), name, description, secret, cost};
}


