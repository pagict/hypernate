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

      auto& tables = config[section_schema][subsec_tables];
      for (int i = 0; i < tables.size(); ++i) {
        _tables.push_back(hyper_table(tables.at(i)));
      }
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
      auto sql = find_table(object.class_name()).make_update_sql(object);
      std::cout << sql << std::endl;
      _cached_transactions.push_back(std::make_pair(sql, object.default_hook));
    }

    void connection::save(persistent_object &object)
    {
      auto sql = find_table(object.class_name()).make_insert_sql(object);
      _cached_transactions.push_back(std::make_pair(sql, object.save_hook));
    }

    void connection::remove(persistent_object &object)
    {
      auto sql = find_table(object.class_name()).make_delete_sql(object);
      _cached_transactions.push_back(std::make_pair(sql, object.remove_hook));
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