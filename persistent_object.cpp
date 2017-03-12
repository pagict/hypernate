//
// Created by PengPremium on 17/2/20.
//

#include "persistent_object.h"
#include "connection.h"

#include <json.hpp>

namespace hypernate {

    void  persistent_object::set_value(const string &key, json value)
    {
        (*_data)[key] = value;
    }

    auto persistent_object::get_value(const string &key) const -> decltype(_data.get()->at(key))
    {
        return _data.get()->at(key);
    }

    persistent_object::persistent_object(connection* conn)
        : _data(std::shared_ptr<nlohmann::json>(new nlohmann::json()))
        , is_created(true)
    {
        if (!conn)  throw std::invalid_argument("null pointer of connection.");
        _internal_table = conn->find_table(class_name());
    }

    persistent_object::persistent_object(shared_ptr<connection> conn)
        : _data(shared_ptr<nlohmann::json>(new nlohmann::json()))
        , is_created(true)
    {
        if (!conn)  throw std::invalid_argument("null pointer of connection.");
        _internal_table = conn->find_table(class_name());
    }
}