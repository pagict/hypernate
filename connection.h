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

      void save(const persistent_object& object);
      void update(const persistent_object& object);
      void remove(const persistent_object& object);
//      void save_or_update(const persistent_object& object);

      template <typename T>
      vector<T> query(const T& object, const unordered_set<string>& exclude_fields) {
        static_assert(std::is_base_of<persistent_object, T>::value,
                      "should be a persistent_object subclass ");

        auto sql = make_query_sql(object, exclude_fields);
        shared_ptr<sql::PreparedStatement> pstmt(this->_con.get()->prepareStatement(sql));
        shared_ptr<sql::ResultSet> rs(pstmt->executeQuery());

        vector<T> list;
        while (rs->next()) {
          T t;
          columns cols = tables.at(object.class_name());
          for (auto &col : cols) {
            json j;
            auto field_name = col.at(key_col_field);
            auto col_name = col.at(key_col_column);
            auto db_type = col.at(key_col_database_type);
            t.set_value(field_name, get_column_data(db_type, col_name, rs));
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

      std::vector<string> _cached_transactions;

      static sql::Driver *_driver;
      shared_ptr<sql::Connection> _con;

      typedef std::unordered_map<string, string> column_attr;
      typedef std::vector<column_attr>  columns;
      std::unordered_map<string, columns> tables;

      void build_tables_attr(const json& sec_schema);

      const string make_insert_sql(const persistent_object& object);
      const string make_update_sql(const persistent_object& object);
      const string make_delete_sql(const persistent_object& object);
      const string make_query_sql(const persistent_object& object, std::unordered_set<string> exclude_fields);

//      void insert(const persistent_object& object);

      /**
       * @brief Find the class's primary field name, fill it in `ret_field_name`.
       * @param class_name the class which contains the primary key.
       * @param ret_field_name a return field. field name of a primary key would returned.
       * @return indicates whether there is a primary column or not.
       */
      inline bool primary_field_name(const string& class_name, string& ret_field_name)
      {
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
      inline const string column_name(const string& class_name, const string& field_name) const {
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
      inline const string field_name(const string& class_name, const string& column_name) const {
        auto cols = tables.at(class_name);
        for(auto &col : cols) {
          if (col.at(key_col_column).compare(column_name) == 0) {
            return col.at(key_col_field);
          }
        }

        return "";
      }

      bool execute_prepared_statement(const string &sql);

      json get_column_data(const string& type, const string& column_name, shared_ptr<sql::ResultSet> rs);
  };

}

#endif //HYPERNATE_CONNECTION_H
