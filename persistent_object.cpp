//
// Created by PengPremium on 17/2/20.
//

#include "persistent_object.h"

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

    persistent_object::persistent_object()
        : _data(std::shared_ptr<nlohmann::json>(new nlohmann::json()))
    {
    }
}