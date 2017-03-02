//
// Created by PengPremium on 17/2/20.
//

#include "connection.h"

#include <cppconn/prepared_statement.h>

namespace hypernate {
    sql::Driver* connection::_driver = get_driver_instance();

    connection::connection(const json &configuration)
    {
      const json config = configuration[section_configuration];
      const json connection_section = config[section_connection];
      _url = connection_section[key_url];
      _username = connection_section[key_username];
      _password = connection_section[key_password];
      _schema = connection_section[key_schema];

      if (!_driver) {
        throw std::system_error(std::make_error_code(std::errc::no_such_device));
      }

      _con = shared_ptr<sql::Connection>(_driver->connect(_url, _username, _password));

      if (!_con) {
        throw std::invalid_argument("can't make a connection");
      }
      _con->setSchema(_schema);

      build_tables_attr(config[section_schema]);
    }


    const string connection::make_insert_sql(const persistent_object &object){
      string table_name = object.class_name();
      columns my_cols = this->tables[table_name];

      string sql = "INSERT INTO `" + _schema + "`.`" + table_name + "` (";
      string value;
      for(int i = 0; i < my_cols.size() - 1; ++i) {
        sql += (my_cols.at(i)[key_col_column] + ", ");
        string col_name = my_cols.at(i)[key_col_field];
        value += json(object.get_value(col_name)).dump() + ", ";
      }
      sql += ((my_cols.at(my_cols.size() - 1)[key_col_column]) + ") VALUES(");

      string type = my_cols.at(my_cols.size()-1)[key_col_class_type];
      string key = my_cols.at(my_cols.size() - 1)[key_col_field];
      value += json(object.get_value(key)).dump() + ");";

      sql += value;

      std::cout << sql << std::endl;
      return sql;
    }

    const string connection::make_update_sql(const persistent_object& object) {
      auto table_name = object.class_name();
      string sql = "UPDATE `" + _schema + "`.`" + table_name + "` SET ";
      string primary_field = "";
      if (primary_field_name(table_name, primary_field) == false) {
        return "";
      }

      for(auto &col : tables.at(table_name)) {
        auto field_name = col.at(key_col_field);
        if (field_name.compare(primary_field) != 0) {
          sql.append(' ' + column_name(table_name, field_name) + '=' + object.get_value(field_name).dump() + ',');
        }
      }
      // remove last period-sign
      if (sql.at(sql.length() - 1) == ',') {
        sql = sql.substr(0, sql.length() - 1);
      }
      sql.append(" WHERE " + column_name(table_name, primary_field) + '=' + object.get_value(primary_field).dump() + ";");
      std::cout << sql;

      return sql;
    }

    const string connection::make_delete_sql(const persistent_object &object)
    {
      auto table_name = object.class_name();
      string sql = "DELETE FROM `" + _schema + "`.`" + table_name + "` WHERE ";
      string primary_field;
      if (primary_field_name(table_name, primary_field) == false) {
        return "";
      }
      sql.append(column_name(table_name, primary_field)).append("=");
      sql.append(object.get_value(primary_field).dump());
      sql.append(";");
      return sql;
    }

    const string connection::make_query_sql(const persistent_object &object,
                                            const unordered_set<string>& exclude_fields,
                                            MatchMode matchMode)
    {
      auto class_name = object.class_name();
      string sql = "SELECT * FROM `" + _schema + "`.`" + class_name + "`";
      columns cols = tables.at(class_name);
      for(auto &col : cols) {
        auto field_name = col.at(key_col_field);
        if (exclude_fields.find(field_name) == exclude_fields.end()) {
          if (sql.at(sql.length()-1) == '`')  sql.append(" WHERE ");

          sql.append(col.at(key_col_column) + match_operator(matchMode, object.get_value(field_name)) + " AND ");
        }
      }

      if (sql.at(sql.length() - 1) != '`') {
        size_t cut_idx = sql.find_last_not_of("AND ") + 1;
        sql = sql.substr(0, cut_idx);
      }
      sql.append(";");
      return sql;
    }

    bool connection::execute_prepared_statement(const string &sql)
    {
      bool result = false;
      shared_ptr<sql::PreparedStatement> psmt(_con.get()->prepareStatement(sql));
      try {
        result = !psmt->execute();
      } catch (const sql::SQLException& e) {
        std::cerr << e.what();
      }
      return result;
    }

    void connection::update(const persistent_object& object)
    {
      auto sql = make_update_sql(object);
      _cached_transactions.push_back(std::make_pair(sql, object.default_hook));
    }

    void connection::save(persistent_object &object)
    {
      auto sql = make_insert_sql(object);
      _cached_transactions.push_back(std::make_pair(sql, object.save_hook));
    }

    void connection::remove(persistent_object &object)
    {
      auto sql = make_delete_sql(object);
      _cached_transactions.push_back(std::make_pair(sql, object.remove_hook));
    }

    void connection::build_tables_attr(const json &sec_schema)
    {
      json tables = sec_schema[subsec_tables];
      for (int i = 0; i < tables.size(); ++i) {
        json table = tables.at(i);

        string name = table[key_table_name];
        json array_columns = table[key_table_columns];
        columns cols;
        for (int j = 0; j < array_columns.size(); ++j) {
          json col = array_columns.at(j);
          column_attr attr;
          string v = col[key_col_field];
          attr.emplace(key_col_field, v);
          v = col[key_col_column];  attr.emplace(key_col_column, v);
          v = col[key_col_class_type]; attr.emplace(key_col_class_type, v);
          v = col[key_col_database_type]; attr.emplace(key_col_database_type, v);
          auto isPrim = col.find(key_col_primary);
          if (isPrim == col.end()) {
            attr.emplace(key_col_primary, "false");
          } else {
            bool tmp = *isPrim;
            attr.emplace(key_col_primary, tmp ? "true" : "false");
          }

          cols.push_back(attr);
        }

        this->tables.emplace(std::make_pair(name, cols));
      }
    }

    void connection::begin_transaction()
    {
      _cached_transactions.clear();
    }

    bool connection::commit()
    {
      bool total_result = true;
      for(auto sql_hook : _cached_transactions) {
        bool result = execute_prepared_statement(sql_hook.first);
        if (result) sql_hook.second();
        total_result = total_result && result;
      }
      _cached_transactions.clear();

      return total_result;
    }

    json connection::get_column_data(const string &type, const string& column_name, shared_ptr<sql::ResultSet> rs)
    {
      if (type.find("int") != string::npos) {
        return rs->getInt(column_name);
      }
      if (type.find("varchar") != string::npos || type.find("text") != string::npos) {
        return rs->getString(column_name);
      }
      if (type.find("double") != string::npos) {
        return rs->getDouble(column_name);
      }

      return json();
    }
}