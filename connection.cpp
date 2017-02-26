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

    bool connection::excute_prepared_statement(const string &sql)
    {
      bool result = false;
      shared_ptr<sql::PreparedStatement> psmt(_con.get()->prepareStatement(sql));
      try {
        result = psmt->execute();
      } catch (const sql::SQLException& e) {
        std::cerr << e.what();
      }
      return result;
    }

    bool connection::insert(const persistent_object &object)
    {
      const string sql = make_insert_sql(object);
      return excute_prepared_statement(sql);
    }

    bool connection::update(const persistent_object &object)
    {
      const string sql = make_update_sql(object);
      return excute_prepared_statement(sql);
    }

    bool connection::save(const persistent_object &object)
    {
      return insert(object);
    }

    bool connection::save_or_update(const persistent_object &object)
    {
      if (save(object)) {
        return true;
      } else {
        return update(object);
      }
    }

    void connection::build_tables_attr(const json &sec_schema)
    {
      json tables = sec_schema[subsec_tables];
      for(int i = 0; i < tables.size(); ++i) {
        json table = tables.at(i);

        string name = table[key_table_name];
        json array_columns = table[key_table_columns];
        columns cols;
        for(int j = 0; j < array_columns.size(); ++j) {
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
}