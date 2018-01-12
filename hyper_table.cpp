//
// Created by PengPremium on 17/3/5.
//

#include "hyper_table.h"
#include "persistent_object.h"
#include "configuration_keys.h"
namespace hypernate
{
hyper_table::hyper_table(const json& table_config) {
  table_name = table_config[key_table_name].get<string>();

  for(auto col : table_config.at(key_table_columns)) {
    shared_ptr<hyper_column> column(new hyper_column(col));
    if (column->is_primary_column())  primary_column = column;
    columns.push_back(column);
  }
};
}
