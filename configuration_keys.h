//
// Created by PengPremium on 17/2/20.
//

#ifndef HYPERNATE_CONFIGURATION_KEY_H
#define HYPERNATE_CONFIGURATION_KEY_H

#include <string>

namespace hypernate {
    using std::string;

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
}
#endif //HYPERNATE_CONFIGURATION_KEY_H
