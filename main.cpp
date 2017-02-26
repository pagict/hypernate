#include <iostream>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include <json.hpp>
#include <fstream>
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
  cout << pt.class_name() << endl;
  pt.set_value("index", 363);
  pt.set_value("text", "updated");
  conn.save(pt);

  exit(EXIT_SUCCESS);


  try
  {
    sql::Driver *driver;
    sql::Connection *con;
    sql::ResultSet *rs;
    sql::PreparedStatement *pstmt;

    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:32771", "root", "root");
    con->setSchema("new_schema");

    std::string create_tbl = "CREATE TABLE `new_schema`.`new_table2` ( \
                            `idnew_table2` INT NOT NULL, \
                            `new_table2col` VARCHAR(45) NULL, \
                              PRIMARY KEY (`idnew_table2`));";
    std::string drop_tbl = "drop table `new_schema`.`new_table2`;";
    std::string insert_ = "insert into new_table2 values(5, \"8888\");";
    std::string query_ = "select * from new_table2;";

    pstmt = con->prepareStatement(create_tbl);
    bool res = pstmt->execute();
    if (!res) {
      cerr << "Create table failed!" << endl;
      exit(1);
    }

    pstmt = con->prepareStatement("SELECT * from new_table;");
    rs = pstmt->executeQuery();

    /* Fetch in reverse = descending order! */
    rs->afterLast();
    while (rs->previous())
      cout << "\t... MySQL counts: " << rs->getInt("idnew_table") << endl;
    delete rs;
  } catch (sql::SQLException e)
  {
    cerr << e.what() << endl;
  }
  return 0;
}