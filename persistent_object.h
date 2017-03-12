//
// Created by PengPremium on 17/2/20.
//

#ifndef HYPERNATE_PERSISTENT_OBJECT_H
#define HYPERNATE_PERSISTENT_OBJECT_H

#include <string>
#include <memory>
#include <unordered_map>

#include <json.hpp>
#include "hyper_table.h"
namespace hypernate
{
    using std::string;
    using std::shared_ptr;
    using nlohmann::json;

    class connection;
    class hyper_table;
    class hyper_column;

    class persistent_object {
     public:
      typedef std::function<void(void)> operation_hook;
     public:
      virtual const string class_name() const {
        return "persistent_object";
      }

     private:
      std::shared_ptr<nlohmann::json > _data;
      std::unordered_map<string, shared_ptr<persistent_object>> _to_one_objects;

      friend class connection;
      bool is_created;
      operation_hook save_hook = [&] () { is_created = false;  };
      operation_hook remove_hook = [&] () { is_created = true; };
      operation_hook query_hook = [&] () { is_created = false; };
      operation_hook default_hook = [&] () {};

      shared_ptr<hyper_table> _internal_table;
     public:
      persistent_object(connection* conn = nullptr);
      persistent_object(shared_ptr<connection> conn);


      void set_value(const string& key, json value);

      auto get_value(const string& key) const -> decltype(_data.get()->at(key));

      template<typename T>
      shared_ptr<T&> get_object(const string& key) {
        static_assert(std::is_base_of<persistent_object, T>::value, "template argument should be a subclass of persistent_object");

        auto col = _internal_table->find_column_by_field_name(key);
        if (col->is_one_to_many_column() || col->is_one_to_one_column()) {
          return _to_one_objects[col->field_name];
        }
        return nullptr;
      }

      inline void set_object(const string& key, shared_ptr<persistent_object> obj) {
        _to_one_objects[key] = obj;
      }
    };
}

#endif //HYPERNATE_PERSISTENT_OBJECT_H
