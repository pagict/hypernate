//
// Created by PengPremium on 17/3/5.
//

#include "hyper_column.h"
#include "configuration_keys.h"

namespace hypernate {
hyper_column::hyper_column(const json &column_config)
    : is_primary(false)
{
  field_name = column_config.at(key_col_field);
  object_type = column_config.at(key_col_class_type);
  if (column_config.find(key_col_column) != column_config.end()) {
    column_name = column_config.at(key_col_column);
  }
  if (column_config.find(key_col_database_type) != column_config.end()) {
    database_type = column_config.at(key_col_database_type);
  }

  is_primary = (column_config.find(key_col_primary) != column_config.end() && column_config[key_col_primary]);
  is_one2one = (column_config.find(key_col_one_to_one) != column_config.end() && column_config[key_col_one_to_one]);
  is_one2many = (column_config.find(key_col_one_to_many) != column_config.end() && column_config[key_col_one_to_many]);
  is_many2one = (column_config.find(key_col_many_to_one) != column_config.end() && column_config[key_col_many_to_one]);
}
}