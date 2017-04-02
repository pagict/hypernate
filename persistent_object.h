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


      const string make_insert_sql() {
        string table_name = class_name();

        string sql = "INSERT INTO " + table_name + " (";
        string value;
        auto columns = _internal_table->columns;
        for(int i = 0; i < columns.size() - 1; ++i) {
          sql += (columns[i]->column_name + ", ");
          string col_name = columns[i]->field_name;
          value += json(get_value(col_name)).dump() + ", ";
        }
        sql += (columns.at(columns.size() - 1)->column_name + ") VALUES(");

        string type = columns.at(columns.size() - 1)->get_object_type();
        string key = columns.at(columns.size() - 1)->field_name;
        value += json(get_value(key)).dump() + ");";

        sql += value;

        std::cout << sql << std::endl;
        return sql;
      }


      const string make_update_sql() {
        string sql = "UPDATE " + class_name() + " SET ";
        string primary_field = _internal_table->get_primary_column()->field_name;

        for(auto &col : _internal_table->columns) {
          auto field_name = col->field_name;
          if (field_name.compare(primary_field) != 0) {
            sql.append(' ' + col->column_name + '=' + get_value(field_name).dump() + ',');
          }
        }
        // remove last period-sign
        if (sql.at(sql.length() - 1) == ',') {
          sql = sql.substr(0, sql.length() - 1);
        }
        sql.append(" WHERE " + _internal_table->get_primary_column()->column_name + '=' + get_value(primary_field).dump() + ";");
        std::cout << sql;

        return sql;
      }


      const string make_delete_sql()
      {
        string sql = "DELETE FROM " + class_name() + " WHERE ";
        string primary_field = _internal_table->get_primary_column()->field_name;
        sql.append(_internal_table->get_primary_column()->column_name).append("=");
        sql.append(get_value(primary_field).dump());
        sql.append(";");
        return sql;
      }

      const string make_query_sql(const unordered_set<string>& exclude_fields,
                                  const match_mode_t mode) const
      {
        string sql = "SELECT * FROM " + class_name();
        const string sql_prefix = sql;
        for(auto &col : _internal_table->columns) {
          auto field_name = col->field_name;
          if (exclude_fields.find(field_name) == exclude_fields.end()) {
            if (sql.compare(sql_prefix) == 0)  sql.append(" WHERE ");

            sql.append(col->column_name + _internal_table->match_operator(mode, get_value(field_name)) + " AND ");
          }
        }

        if (sql.at(sql.length() - 1) != '`') {
          size_t cut_idx = sql.find_last_not_of("AND ") + 1;
          sql = sql.substr(0, cut_idx);
        }
        sql.append(";");
        return sql;
      }

     public:
          persistent_object(const string&, connection* conn = nullptr);
          persistent_object(const string&, shared_ptr<connection> conn);
    };
}

#endif //HYPERNATE_PERSISTENT_OBJECT_H
