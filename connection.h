//
// Created by PengPremium on 17/2/20.
//

#ifndef HYPERNATE_CONNECTION_H
#define HYPERNATE_CONNECTION_H
#include <string>
#include <memory>
#include <unordered_map>
#include <json.hpp>
#include <cppconn/driver.h>

#include "persistent_object.h"

namespace hypernate {
    using std::string;
    using std::shared_ptr;
    using nlohmann::json;


  class connection {
    public:
      connection(const json& connection_section);

      bool save(const persistent_object& object);

    private:
      string  _url;
      string  _username;
      string  _password;
      string  _schema;

      static sql::Driver *_driver;
      shared_ptr<sql::Connection> _con;

      typedef std::unordered_map<string, string> column_attr;
      typedef std::vector<column_attr>  columns;
      std::unordered_map<string, columns> tables;

      void build_tables_attr(const json& sec_schema);

      const string make_insert_sql(const persistent_object& object);
  };

}

#endif //HYPERNATE_CONNECTION_H
