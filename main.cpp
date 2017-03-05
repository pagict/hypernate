#include <iostream>
#include <fstream>

#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <json.hpp>

#include "connection.h"
#include "plain_table.h"
#include "school.h"

using namespace std;
using namespace hypernate;

int main()
{
  ifstream config("orm.json");
  json configs(config);

  connection conn(configs);

  teacher t;
  auto list = conn.query(t, {"id", "name"});
  for(auto &item : list ) {
    cout << "{ id:" << item.get_value("id") << ", name:" << item.get_value("name") << "}" << endl;
  }

  exit(EXIT_SUCCESS);
}