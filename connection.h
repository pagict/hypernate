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
#include "configuration_keys.h"

namespace hypernate {
    using std::string;
    using std::shared_ptr;
    using nlohmann::json;


  class connection {
    public:
      connection(const json& connection_section);

      bool save(const persistent_object& object);
      bool save_or_update(const persistent_object& object);

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
      const string make_update_sql(const persistent_object& object);

      bool insert(const persistent_object& object);
      bool update(const persistent_object& object);

      /*
       * Find the object primary field name, fill it in `ret_field_name`, the returning value
       * indicating whether the primary column contained.
       */
      bool primary_field_name(const persistent_object& object, string& ret_field_name)
      {
        auto class_name = object.class_name();
        auto columns = tables.at(class_name);
        for(auto col : columns) {
            auto is_primary_string = col.at(key_col_primary);
            if (is_primary_string.compare("true") == 0) {
              ret_field_name = col.at(key_col_field);
              return true;
            }

        }
        return false;
      }

      /**
       * @brief find the column name(in database), by the given field name(in memory object).
       * @param class_name Also table name, which table the field/column belongs to.
       * @param field_name
       * @return column name in database.
       */
      const string column_name(const string& class_name, const string& field_name) const {
        auto cols = tables.at(class_name);
        for(auto &col : cols) {
          if (col.at(key_col_field).compare(field_name) == 0) {
            return col.at(key_col_column);
          }
        }

        return "";
      }

      /**
       * @brief find the field name(in memory object), by the given column name(in database).
       * @param class_name Also table name, which table the field/column belongs to.
       * @param column_name
       * @return field name in memory object.
       */
      const string field_name(const string& class_name, const string& column_name) const {
        auto cols = tables.at(class_name);
        for(auto &col : cols) {
          if (col.at(key_col_column).compare(column_name) == 0) {
            return col.at(key_col_field);
          }
        }

        return "";
      }
  };

}

#endif //HYPERNATE_CONNECTION_H
