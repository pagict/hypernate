//
// Created by PengPremium on 17/2/20.
//

#ifndef HYPERNATE_PERSISTENT_OBJECT_H
#define HYPERNATE_PERSISTENT_OBJECT_H

#include <string>

#include <json.hpp>
namespace hypernate
{
    using std::string;
    using nlohmann::json;

    class persistent_object {
     public:
      typedef std::function<void(void)> operation_hook;
     public:
      virtual const string class_name() const {
        return "persistent_object";
      }

     private:
      std::shared_ptr<nlohmann::json > _data;

      friend class connection;
      bool is_created;
      operation_hook save_hook = [&] () { is_created = false;  };
      operation_hook remove_hook = [&] () { is_created = true; };
      operation_hook query_hook = [&] () { is_created = false; };
      operation_hook default_hook = [&] () {};
     public:
      persistent_object();


      void set_value(const string& key, json value);

      auto get_value(const string& key) const -> decltype(_data.get()->at(key));

    };
}

#endif //HYPERNATE_PERSISTENT_OBJECT_H
