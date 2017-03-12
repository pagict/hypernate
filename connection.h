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
#include <unordered_set>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include "persistent_object.h"
#include "configuration_keys.h"
#include "hyper_table.h"

namespace hypernate {
    using std::string;
    using std::shared_ptr;
    using std::vector;
    using std::unordered_set;
    using std::remove_reference;
    using std::remove_const;
    using nlohmann::json;


  class connection {
    public:
      connection(const json& connection_section);

      void save(persistent_object& object);
      void update(const persistent_object& object);
      void remove(persistent_object& object);

      template <typename T>
      vector<T> query(const T& object,
                      const unordered_set<string>& exclude_fields = {},
                      match_mode_t matchMode = match_mode_exact) {
        static_assert(std::is_base_of<persistent_object, T>::value,
                      "should be a persistent_object subclass ");

        auto table = find_table(object.class_name());
        auto sql = table->make_query_sql(object, exclude_fields, matchMode);
        std::cout << sql << std::endl;
        shared_ptr<sql::PreparedStatement> pstmt(this->_con.get()->prepareStatement(sql));
        shared_ptr<sql::ResultSet> rs(pstmt->executeQuery());

        vector<T> list;
        while (rs->next()) {
          T t(this);
//          columns cols = tables.at(object.class_name());
          auto cols = find_table(object.class_name())->columns;
          for (auto &col : cols) {
            json j;
            auto field_name = col->field_name;
            auto col_name = col->column_name;
            auto db_type = col->get_database_type();

            if (col->is_one_to_one_column()) {
              auto db_col_name = col->column_name;
            } else {
              t.set_value(field_name, get_column_data(db_type, col_name, rs));
            }
            t.query_hook();
          }

          list.push_back(t);
        }
        return list;
      }

      void begin_transaction();
      bool commit();

    private:
      string  _url;
      string  _username;
      string  _password;
      string  _schema;

      std::vector<std::pair<string, persistent_object::operation_hook>> _cached_transactions;
      std::unordered_map<string, persistent_object&> _registered;


      static sql::Driver *_driver;
      shared_ptr<sql::Connection> _con;

      vector<shared_ptr<hyper_table>> _tables;

      bool execute_prepared_statement(const string &sql);

      json get_column_data(const string& type, const string& column_name, shared_ptr<sql::ResultSet> rs);

   public:
      shared_ptr<hyper_table> find_table(const string& class_name)
      {
        for(auto table : _tables) {
          if (table->table_name.compare(class_name) == 0) return table;
        }

        throw std::out_of_range("no such table named '" + class_name + "'");
      }

      template <typename T>
      void register_persistent_object(const T& object) {
        static_assert(std::is_base_of<persistent_object, T>::value,
                      "template argument should be a subclass of persistent_object");
        _registered[object.class_name()] = T();
      }
  };

}

#endif //HYPERNATE_CONNECTION_H
