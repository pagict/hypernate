//
// Created by PengPremium on 17/2/20.
//

#ifndef HYPERNATE_CONFIGURATION_KEY_H
#define HYPERNATE_CONFIGURATION_KEY_H

#include <string>

namespace hypernate {
    enum match_mode_t {
        match_mode_exact,
        match_mode_any,
        match_mode_begin,
        match_mode_end
    };



    static const string  section_configuration = "configuration";
    static const string  section_connection    = "connection";
    static const string  key_url               = "url";
    static const string  key_username          = "username";
    static const string  key_password          = "password";
    static const string  key_schema            = "schema";
    static const string  section_schema        = "schema";
    static const string  subsec_tables         = "tables";
    static const string  key_table_name        = "name";
    static const string  key_table_columns     = "columns";
    static const string  key_col_field         = "field";
    static const string  key_col_column        = "column";
    static const string  key_col_primary       = "primary";
    static const string  key_col_class_type    = "class_type";
    static const string  key_col_database_type = "database_type";
    static const string  key_col_1_2_1         = "one_to_one";
}
#endif //HYPERNATE_CONFIGURATION_KEY_H
