//
// Created by PengPremium on 17/2/20.
//

#ifndef HYPERNATE_CONNECTION_H
#define HYPERNATE_CONNECTION_H
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <json.hpp>
#include <cppconn/driver.h>
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
using nlohmann::json;

/**
 * @brief
 * This class parses configuration file,
 * manages connection with mysql server,
 * doing database-related operations,
 */
class connection {
 public:

  /**
   * @brief construct a connection instance with a configuration object.
   * @param configuration
   */
  connection(const json& configuration);

  /**
   * @brief save an object to the database.
   * @param object
   */
  void save(persistent_object& object);

  /**
   * @brief update an object.
   * @param object
   */
  void update(persistent_object& object);

  /**
   * @brief remove an object from the database.
   * @param object
   */
  void remove(persistent_object& object);

  /**
   * @brief search objects like object given as @param object
   * @param object sample object to be searched
   * @param exclude_fields fields should be ignored
   * @param matchMode fields match mode, one of {exact, any, start, end}
   * @return a list of objects satisfied the criteria as the sample demonstrates.
   */
  vector<shared_ptr<persistent_object>> query(shared_ptr<const persistent_object> object,
                                              const unordered_set<string>& exclude_fields = {},
                                              match_mode_t matchMode = match_mode_exact) {
//    static_assert(std::is_base_of<persistent_object, T>::value,
//                  "should be a persistent_object subclass ");

    // make SQL statement
    auto table = find_table(object->class_name());
    auto sql = object->make_query_sql(exclude_fields, matchMode);

    // do primitive query operation
    shared_ptr<sql::PreparedStatement> pstmt(this->_con.get()->prepareStatement(sql));
    shared_ptr<sql::ResultSet> rs(pstmt->executeQuery());

    vector<shared_ptr<persistent_object>> list;
    // iterate through the result set
    while (rs->next()) {
      // make the object
      shared_ptr<persistent_object> one_query_result = shared_ptr<persistent_object>(new persistent_object(object->class_name(), this));

      // get every column data of the object
      auto cols = find_table(object->class_name())->columns;
      for (auto &col : cols) {
        auto field_name = col->field_name;
        auto col_name = col->column_name;
        auto db_type = col->get_database_type();


        if (col->is_one_to_one_column()) {
          auto db_value = get_column_data(db_type, col_name, rs);
          auto registered_ptr = _registered.find(col->get_object_type())->second;

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
        } else if (col->is_one_to_many_column()) {

        } else {
          one_query_result->set_value(field_name, get_column_data(db_type, col_name, rs));
        }
        one_query_result->query_hook();
      }

      list.push_back(one_query_result);
    }
    return list;
  }

  /**
   * @brief clean the cache of the execution SQL
   */
  void begin_transaction();

  /**
   * @brief execute the SQLs cached before.
   * @return if all SQLs have been successfully executed.
   */
  bool commit();

  /**
   * @brief get the hyper_table object specified by the @param class_name
   * @param class_name
   * @return the hyper_table, or nullptr if not exist.
   */
  shared_ptr<hyper_table> find_table(const string& class_name)
  {
    for(auto table : _tables) {
      if (table->table_name.compare(class_name) == 0) return table;
    }

    return nullptr;
  }

  /**
   * @brief register a persistent_object. This is important particularly when
   * implementing one-to-one or one-to-many relations.
   * Better call it as `register_persistent_object(shared_ptr<derived_object>(new derived_object(conn)))`.
   * @param object
   */
//      template <typename T>
  void register_persistent_object(shared_ptr<persistent_object> object) {
    /* TODO: better consider accept it as a unique_ptr, in function \ implementation, insert a new-created shared_ptr which type deduced. */
//        static_assert(std::is_base_of<persistent_object, T>::value,
//                      "template argument should be a subclass of persistent_object");
    string class_name = object->class_name();

    if (_registered.find(class_name) == _registered.end()) {
      _registered.emplace(class_name, object);
    }
  }

 private:
  /**
   * @brief execute the given SQL.
   * @param sql
   * @return whether the SQL is executed successfully.
   */
  bool execute_prepared_statement(const string &sql);

  /***
   * @brief get data of column @param column_name in type @param type from @param rs
   * @param type specify the data type
   * @param column_name specify the column name
   * @param rs result set retained the data
   * @return data
   */
  json get_column_data(const string& type, const string& column_name, shared_ptr<sql::ResultSet> rs);

  string  _url;
  string  _username;
  string  _password;
  string  _schema;

  std::vector<std::pair<string, persistent_object::operation_hook>> _cached_transactions;
  std::unordered_map<string, shared_ptr<persistent_object>> _registered;

  shared_ptr<sql::Connection> _con;

  vector<shared_ptr<hyper_table>> _tables;


  static sql::Driver *_driver;
};

}

#endif //HYPERNATE_CONNECTION_H
