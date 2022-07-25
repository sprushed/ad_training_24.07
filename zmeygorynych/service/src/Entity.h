//
// Created by Kurisu on 21/07/2022.
//

#ifndef SRC_ENTITY_H
#define SRC_ENTITY_H

#include <cstdint>
#include <string>
class Database;
#include <vector>
using namespace std;
class Entity {
protected:
    uint32_t owner = 0;
    uint32_t id = 0;
    string name;
    string description;
    string secret;
    Database* db;

public:
    explicit Entity(Database* db);
    void setName(const string &name);
    void setDescription(const string &description);
    void setId(uint32_t id);
    void setSecret(const string &secret);
    void setOwner(uint32_t id);
    string getName();
    string getDescription();
    uint32_t getId();
    string getSecret();
    uint32_t getOwner();






};

#endif //SRC_ENTITY_H
