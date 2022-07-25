//
// Created by Kurisu on 22/07/2022.
//
#include "Entity.h"

Entity::Entity(Database *db) {
    this->db = db;
}

void Entity::setName(const string &name) {
    this->name = name;
}

void Entity::setDescription(const string &description) {
    this->description = description;
}

void Entity::setId(uint32_t id) {
    this->id = id;
}

void Entity::setSecret(const string &secret) {
    this->secret = secret;
}

void Entity::setOwner(uint32_t id) {
    this->owner = id;
}

string Entity::getName() {
    return this->name;
}

string Entity::getDescription() {
    return this->description;
}

uint32_t Entity::getId() {
    return this->id;
}

string Entity::getSecret() {
    return this->secret;
}

uint32_t Entity::getOwner() {
    return this->owner;
}

