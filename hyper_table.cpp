//
// Created by PengPremium on 17/3/5.
//

#include "hyper_table.h"
#include "configuration_keys.h"
namespace hypernate
{
    hyper_table::hyper_table(const json& table_config) {
        table_name = table_config[key_table_name];

        for(auto col : table_config.at(key_table_columns)) {
          shared_ptr<hyper_column> column(new hyper_column(col));
          if (column->is_primary_column())  primary_column = column;
          columns.push_back(column);
        }
    };


    const string hyper_table::make_insert_sql(const persistent_object &object){
      string table_name = object.class_name();

      string sql = "INSERT INTO " + table_name + " (";
      string value;
      for(int i = 0; i < columns.size() - 1; ++i) {
        sql += (columns[i]->column_name + ", ");
        string col_name = columns[i]->field_name;
        value += json(object.get_value(col_name)).dump() + ", ";
      }
      sql += (columns.at(columns.size() - 1)->column_name + ") VALUES(");

      string type = columns.at(columns.size() - 1)->get_object_type();
      string key = columns.at(columns.size() - 1)->field_name;
      value += json(object.get_value(key)).dump() + ");";

      sql += value;

      std::cout << sql << std::endl;
      return sql;
    }


    const string hyper_table::make_update_sql(const persistent_object& object) {
      string sql = "UPDATE " + table_name + " SET ";
      string primary_field = primary_column->field_name;

      for(auto &col : columns) {
        auto field_name = col->field_name;
        if (field_name.compare(primary_field) != 0) {
          sql.append(' ' + col->column_name + '=' + object.get_value(field_name).dump() + ',');
        }
      }
      // remove last period-sign
      if (sql.at(sql.length() - 1) == ',') {
        sql = sql.substr(0, sql.length() - 1);
      }
      sql.append(" WHERE " + primary_column->column_name + '=' + object.get_value(primary_field).dump() + ";");
      std::cout << sql;

      return sql;
    }


    const string hyper_table::make_delete_sql(const persistent_object &object)
    {
      string sql = "DELETE FROM " + table_name + " WHERE ";
      string primary_field = primary_column->field_name;
      sql.append(primary_column->column_name).append("=");
      sql.append(object.get_value(primary_field).dump());
      sql.append(";");
      return sql;
    }

    const string hyper_table::make_query_sql(const persistent_object &object,
                                             const unordered_set<string>& exclude_fields,
                                             const match_mode_t mode)
    {
      string sql = "SELECT * FROM " + table_name;
      const string sql_prefix = sql;
      for(auto &col : columns) {
        auto field_name = col->field_name;
        if (exclude_fields.find(field_name) == exclude_fields.end()) {
          if (sql.compare(sql_prefix) == 0)  sql.append(" WHERE ");

          sql.append(col->column_name + match_operator(mode, object.get_value(field_name)) + " AND ");
        }
      }

      if (sql.at(sql.length() - 1) != '`') {
        size_t cut_idx = sql.find_last_not_of("AND ") + 1;
        sql = sql.substr(0, cut_idx);
      }
      sql.append(";");
      return sql;
    }

}