//
// Created by PengPremium on 17/2/20.
//

#include "persistent_object.h"
#include "connection.h"

namespace hypernate {

void  persistent_object::set_value(const string &key, json value)
{
    (*_data)[key] = value;
}

auto persistent_object::get_value(const string &key) const -> decltype(_data.get()->at(key))
{
    return _data.get()->at(key);
}

persistent_object::persistent_object(const string& class_name, connection* conn)
    : _data(std::shared_ptr<nlohmann::json>(new nlohmann::json()))
    , is_created(true)
    , my_class_name(class_name)
{
    if (conn)//  throw std::invalid_argument("null pointer of connection.");
        _internal_table = conn->find_table(class_name);
}

persistent_object::persistent_object(const string& class_name, shared_ptr<connection> conn)
    : _data(shared_ptr<nlohmann::json>(new nlohmann::json()))
    , is_created(true)
    , my_class_name(class_name)
{
    if (conn)  //throw std::invalid_argument("null pointer of connection.");
        _internal_table = conn->find_table(class_name);
}

void persistent_object::set_objects(const string &key, unordered_set<shared_ptr<persistent_object>> set)
{
    _to_many_sets[key] = set;
}
unordered_set<shared_ptr<persistent_object>>& persistent_object::get_objects(const string &key)
{
  auto search = _to_many_sets.find(key);
  if (search !=  _to_many_sets.end()) return search->second;

  unordered_set<shared_ptr<persistent_object>> new_one;
  set_objects(key, new_one);
  return new_one;
}
}