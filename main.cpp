#include <iostream>
#include <fstream>

#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <json.hpp>

#include "connection.h"
#include "plain_table.h"

using namespace std;
using namespace hypernate;

int main()
{
  ifstream config("orm.json");
  json configs(config);

  connection conn(configs);

  plain_table pt;
//  plain_table another;
//  another.set_value("index", 333);
//  conn.begin_transaction();
  pt.set_value("index", 363);
  pt.set_value("text", "updated");
//  conn.remove(another);
//  conn.update(pt);
//  conn.commit();

  auto list = conn.query(pt, { "index"}, MatchMode_ANY);
  for(auto &item : list) {
    cout <<"{ index:" << item.get_value("index") << ", text:" << item.get_value("text") << "}" << endl;
  }

  plain_table another_pt;
  another_pt.set_value("text", "another pt text");
  another_pt.set_value("index", 878);
  conn.begin_transaction();
  conn.save(another_pt);
  conn.commit();

  exit(EXIT_SUCCESS);
}