//
// Created by PengPremium on 17/3/5.
//

#ifndef HYPERNATE_COLUMN_H
#define HYPERNATE_COLUMN_H

#include <json.hpp>
#include <string>
namespace hypernate
{
using std::string;
using nlohmann::json;

class hyper_column {
 public:
  hyper_column(const json& column_config);
  string field_name;            // name in object
  string column_name;           // name in database

  inline const string get_database_type() { return database_type; }
  inline const string get_object_type() { return object_type; }

  inline bool is_primary_column() { return is_primary; }
  inline bool is_one_to_one_column() { return is_one2one; }
  inline bool is_one_to_many_column() { return is_one2many; }

 private:
  bool is_primary;
  string database_type;
  string object_type;

  bool is_one2one;
  bool is_one2many;
};
}

#endif //HYPERNATE_COLUMN_H
