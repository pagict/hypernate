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
    using std::unique_ptr;
    using std::vector;
    using std::unordered_set;
    using std::remove_reference;
    using std::remove_const;
    using nlohmann::json;


  class connection {
    public:
      connection(const json& connection_section);

      void save(persistent_object& object);
      void update(persistent_object& object);
      void remove(persistent_object& object);

      vector<shared_ptr<persistent_object>> query(shared_ptr<const persistent_object> object,
                      const unordered_set<string>& exclude_fields = {},
                      match_mode_t matchMode = match_mode_exact) {
//        static_assert(std::is_base_of<persistent_object, T>::value,
//                      "should be a persistent_object subclass ");

        auto table = find_table(object->class_name());
        auto sql = object->make_query_sql(exclude_fields, matchMode);
        std::cout << sql << std::endl;
        shared_ptr<sql::PreparedStatement> pstmt(this->_con.get()->prepareStatement(sql));
        shared_ptr<sql::ResultSet> rs(pstmt->executeQuery());

        vector<shared_ptr<persistent_object>> list;
        while (rs->next()) {
          shared_ptr<persistent_object> one_query_result = shared_ptr<persistent_object>(new persistent_object(object->class_name(), this));
//          columns cols = tables.at(object.class_name());
          auto cols = find_table(object->class_name())->columns;
          for (auto &col : cols) {
            json j;
            auto field_name = col->field_name;
            auto col_name = col->column_name;
            auto db_type = col->get_database_type();

            if (col->is_one_to_one_column()) {
              auto db_col_name = col->column_name;
              auto db_value = get_column_data(db_type, db_col_name, rs);
              auto registered_ptr = _registered.find(col->get_object_type())->second;

//              auto _to_one_object = shared_ptr<typename std::decay<decltype(registered_class)>::type>(new typename std::decay<decltype(registered_class)>::type(this));
//              auto primary_name = _to_one_object->_internal_table->get_primary_column()->field_name;
//              _to_one_object->set_value(field_name, db_value);
//              auto connected_object = this->query()

              auto class_name = col->get_object_type();
              auto query_sample = shared_ptr<std::remove_pointer<decltype(registered_ptr.get())>::type>(new std::remove_pointer<decltype(registered_ptr.get())>::type(class_name, this));
              auto primary_name = registered_ptr->_internal_table->get_primary_column()->field_name;
              query_sample->set_value(primary_name, db_value);
              unordered_set<string> exclusions;
              for (auto &col: query_sample->_internal_table->columns) {
                if (col->is_primary_column()) continue;
                exclusions.insert(col->field_name);
              }
              one_query_result->set_object(field_name, this->query(query_sample, exclusions).front());
            } else {
              one_query_result->set_value(field_name, get_column_data(db_type, col_name, rs));
            }
            one_query_result->query_hook();
          }

          list.push_back(one_query_result);
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
      std::unordered_map<string, shared_ptr<persistent_object>> _registered;


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

//      template <typename T>
      void register_persistent_object(shared_ptr<persistent_object> object) {
//        static_assert(std::is_base_of<persistent_object, T>::value,
//                      "template argument should be a subclass of persistent_object");
//        _registered[object.class_name()] = T();
        string class_name = object->class_name();

        if (_registered.find(class_name) == _registered.end()) {
          _registered.emplace(class_name, object);
        }
      }
  };

}

#endif //HYPERNATE_CONNECTION_H
