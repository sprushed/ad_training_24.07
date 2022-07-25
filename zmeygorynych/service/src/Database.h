//
// Created by Kurisu on 21/07/2022.
//

#ifndef SRC_DATABASE_H
#define SRC_DATABASE_H
#include <pqxx/pqxx>
#include "User.h"
#include <ranges>
#include <variant>
#include <cstdint>
#include <fmt/format.h>
#include "consts.h"
using namespace std;
class Database {
private:
    pqxx::connection* c;
    pqxx::work* txn;
    bool privileged = false;
    void connect();
public:
    [[nodiscard]] bool isPrivileged() const;
    Database();
    explicit Database(bool privileged);
    template<typename T>
    T getItem(string table, unsigned int id, string column) {
        return this->txn->query_value<T>(fmt::format("SELECT {} from {} where id={};", column, table, id));
    }
    uint32_t maxId(const string& table);
    void txnRefresh();
    bool exists(const string& table, map<string, string> criteria);
    uint32_t findId(const string& table, map<string, string> criteria);
    vector<uint32_t> findIds(const string& table, map<string, string> criteria);
    uint32_t insertItem(const string& table, map<string, variant<std::string, const char *, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>> data);
    void updateItem(const string& table, uint32_t id, map<string, variant<std::string, const char *, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>> data);
    bool idExists(const string& table, uint32_t id);

    string esc(string basicString);
};

#endif //SRC_DATABASE_H
