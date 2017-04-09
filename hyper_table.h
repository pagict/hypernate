//
// Created by PengPremium on 17/3/5.
//

#ifndef HYPERNATE_TABLE_H
#define HYPERNATE_TABLE_H

#include <vector>
#include <unordered_set>

#include <json.hpp>

#include "hyper_column.h"
#include "configuration_keys.h"

namespace hypernate
{
using std::string;
using std::shared_ptr;
using std::vector;
using std::unordered_set;

class persistent_object;

class hyper_table {
 public:
  hyper_table(const json& table_config);
  string table_name;
  vector<shared_ptr<hyper_column>> columns;

  inline shared_ptr<hyper_column> find_column_by_column_name(const string& col_name) {
    for(auto col : columns) {
      if (col->column_name.compare(col_name) == 0) {
        return col;
      }
    }
    return nullptr;
  }

  inline shared_ptr<hyper_column> find_column_by_field_name(const string& field_name) {
    for(auto col : columns) {
      if (col->field_name.compare(field_name) == 0) {
        return col;
      }
    }
    return nullptr;
  }

  inline shared_ptr<hyper_column> get_primary_column() { return primary_column; }

  static const string match_operator(match_mode_t mode, const json& value) {
    if (value.is_number()) return "=" + value.dump();

    if (value.is_string()) {
      switch (mode) {
        case match_mode_exact:
          return "=" + value.dump();
        case match_mode_begin:
          return " LIKE '" + value.get<string>() + "%'";
        case match_mode_end:
          return " LIKE '%" + value.get<string>() + "'";
        case match_mode_any:
          return " LIKE '%" + value.get<string>() + "%'";
      }
    }
    return "";
  }
 private:
  shared_ptr<hyper_column> primary_column;
};
}

#endif //HYPERNATE_TABLE_H
