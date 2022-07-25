//
// Created by Kurisu on 21/07/2022.
//

#include "Database.h"
#include <fmt/format.h>

#include <utility>
#include "util.h"
void Database::connect() {
    try {
        this->c = new pqxx::connection(fmt::format("{}", fmt::join(vector<string>({
            fmt::format("user={}", (this->privileged) ? "admin" : "unpriv"),
            fmt::format("password={}", (this->privileged) ? "ju57451mpl3p455w0rd" : "51mpl3p455w0rdbu7f0ru53r"),
            "host=postgres",
            "dbname=zmeydb"
        }), " ")));
    } catch (const exception& e){
        failure("Database::connect connection failure", __LINE__);
        throw e;
    }
    this->txn = new pqxx::work(*this->c);
}

bool Database::isPrivileged() const {
    return this->privileged;
}

Database::Database() {
    this->privileged = false;
    this->connect();
}

Database::Database(bool privileged) {
    this->privileged = privileged;
    this->connect();
}



uint32_t Database::findId(const string& table, map<string, string> criteria) {
    //TODO: Find more elegant way to do it
    string statement = fmt::format("{}", fmt::join([&criteria](){
        vector<string> parts;
        for (const auto& v:  criteria) parts.push_back(fmt::format("{}='{}'", v.first, v.second));
        return parts;
    }(), " AND "));
    uint32_t id;
    try {
        string q = fmt::format("SELECT id from {} WHERE {};", table, statement);
        debug(q);
        id = txn->query_value<uint32_t>(q);
    } catch (const exception& e) {
        failure("Database::findId txn->query_value failure", __LINE__);
        failure(fmt::format("e.what(): {}", e.what()), __LINE__);
        txnRefresh();
        throw e;
    };
    return id;
}

uint32_t Database::insertItem(const string& table, map<string, variant<std::string, const char *, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>> data) {
    if (!this->privileged) throw std::runtime_error("Internal error");
    string fields = fmt::format("{}", fmt::join([&data](){
        vector<string> parts;
        for (const auto& v: data) parts.push_back(v.first);
        return parts;
    }(), ", "));
    string values = fmt::format("{}", fmt::join([this, &data](){
        vector<string> parts;
        for (const auto& v: data) parts.push_back("'"+this->c->esc(
                std::visit([](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_arithmetic_v<T>) return std::to_string(arg);
                    else return std::string(arg);
                }, v.second))+"'");
        return parts;
    }(), ", "));
    sleep(1);
    auto res = this->txn->query_value<uint32_t>(fmt::format("INSERT INTO {} ({}) VALUES ({}) RETURNING id;", table, fields, values));
    this->txn->commit();
    this->txnRefresh();
    return res;
}

bool Database::idExists(const string &table, uint32_t id) {
    try {
        this->findId(table, {{"id", to_string(id)}});
        return true;
    } catch (const exception& e){
        return false;
    }
}

void Database::updateItem(const string& table, uint32_t id, map<string, variant<std::string, const char *, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>> data){
    if (!this->privileged) throw std::runtime_error("Internal error");
    if (!this->idExists(table, id)) throw std::runtime_error("No such item");
    vector<string> fields = [&data](){
        vector<string> parts;
        for (const auto& v: data) parts.push_back(v.first);
        return parts;
    }();
    vector<string> values = [this, &data](){
        vector<string> parts;
        for (const auto& v: data) parts.push_back(this->c->esc(
                    std::visit([](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_arithmetic_v<T>) return std::to_string(arg);
                        else return std::string(arg);
                    }, v.second)));
        return parts;
    }();
    string statement = fmt::format("{}", fmt::join([criteria = [fields, values](){
        map<string, string> m;
        for (uint32_t i = 0; i < fields.size(); i++) m[fields[i]] = values[i];
        return m;
    }()](){
        vector<string> parts;
        for (const auto& v:  criteria) parts.push_back(fmt::format("{}='{}'", v.first, v.second));
        return parts;
    }(), ", "));
    this->txn->exec0(fmt::format("UPDATE {} SET {} WHERE id={}", table, statement, id));
    this->txn->commit();
    this->txnRefresh();


}

vector<uint32_t> Database::findIds(const string &table, map<string, string> criteria) {
    string statement = fmt::format("{}", fmt::join([this, &criteria](){
        vector<string> parts;
        for (const auto& v:  criteria) parts.push_back(fmt::format("{}='{}'", v.first, this->c->esc(v.second)));
        return parts;
    }(), " AND "));
    vector<uint32_t> res;
    try {
        string q = fmt::format("SELECT id from {} WHERE {};", table, statement);
        debug(q);
        pqxx::result r = this->txn->exec(q);
        for (auto const& row: r){
            res.push_back(row[0].as<uint32_t>());
        }
    } catch (const exception& e) {
        failure("Database::findId txn->query_value failure", __LINE__);
        failure(fmt::format("e.what(): {}", e.what()), __LINE__);
        throw e;
    };
    return res;
}

bool Database::exists(const string &table, map<string, string> criteria) {
    try {
        findId(table, std::move(criteria));
        return true;
    } catch (const exception& e){
        return false;
    }
}

uint32_t Database::maxId(const string &table) {
    uint32_t id;
    try {
        string q = fmt::format("SELECT MAX(id) from {};", table);
        debug(q);
        id = txn->query_value<uint32_t>(q);
    } catch (const exception& e) {
        failure("Database::findId txn->query_value failure", __LINE__);
        failure(fmt::format("e.what(): {}", e.what()), __LINE__);
        id = 0;
    };
    return id;

}

void Database::txnRefresh() {
    delete this->txn;
    this->txn = new pqxx::work(*this->c);

}

string Database::esc(string basicString) {
    return this->c->esc(basicString);
}








