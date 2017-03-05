//
// Created by PengPremium on 17/3/5.
//

#ifndef HYPERNATE_TABLE_H
#define HYPERNATE_TABLE_H

#include <json.hpp>
#include <vector>
#include <unordered_set>
#include "hyper_column.h"
#include "persistent_object.h"
#include "configuration_keys.h"

namespace hypernate
{
    using std::string;
    using std::shared_ptr;
    using std::vector;
    using std::unordered_set;

    class hyper_table {
     public:
      hyper_table(const json& table_config);
      string table_name;
      vector<shared_ptr<hyper_column>> columns;

      inline shared_ptr<hyper_column> get_primary_column() { return primary_column; }
      const string make_insert_sql(const persistent_object& object);
      const string make_update_sql(const persistent_object& object);
      const string make_delete_sql(const persistent_object& object);
      const string make_query_sql(const persistent_object& object,
                                  const unordered_set<string>& exclusions = {},
                                  const match_mode_t mode = match_mode_exact);

      const string match_operator(match_mode_t mode, const json& value) const {
        if (value.is_number()) return value.dump();

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
