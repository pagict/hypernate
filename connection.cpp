//
// Created by PengPremium on 17/2/20.
//

#include <cppconn/prepared_statement.h>
#include "connection.h"
#include "configuration_keys.h"
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

    bool connection::save(const persistent_object &object)
    {
      const string sql = make_insert_sql(object);
      sql::PreparedStatement *pstmt = this->_con.get()->prepareStatement(sql);
      bool save_result = false;
      try {
        save_result = pstmt->execute();
      } catch (sql::SQLException e) {
        std::cout << e.what();
      }

      return save_result;
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