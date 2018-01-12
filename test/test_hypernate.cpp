//
// Created by PengPremium on 17/3/7.
//
#include <fstream>
#include <iostream>

#include <gtest/gtest.h>
#include <json.hpp>

#include "plain_table.h"
#include "../connection.h"
#include "school.h"

using namespace std;
using json = nlohmann::json;

TEST(simple_insert_query_update_remove, _plain_table) {
  ifstream config("orm.json");
  json configs(config);

  shared_ptr<hypernate::connection> conn(new hypernate::connection(configs));
  plain_table pt(conn);
  pt.set_value("index", 45);
  pt.set_value("text", "hello insert");
  conn->begin_transaction();
  conn->save(pt);
  conn->commit();

  shared_ptr<plain_table> pt_ptr(new plain_table(conn));
  pt_ptr->set_value("index", 45);
  pt_ptr->set_value("text", "hello insert");
  auto from_db = conn->query(pt_ptr, hypernate::fields_exclude);

  ASSERT_GE(from_db.size(), 0);

  const string updated = "updated insertion";
  pt.set_value("text", updated);
  conn->begin_transaction();
  conn->update(pt);
  conn->commit();

  pt_ptr->set_value("text", updated);
  from_db = conn->query(pt_ptr, hypernate::fields_exclude, {"text"});
  const string text_database = from_db.front()->get_value("text").dump();

  EXPECT_EQ(from_db.size(), 1);
  string expect_db_name("\"");
  expect_db_name.append(updated).append(1, '\"');
  ASSERT_STREQ(text_database.c_str(), expect_db_name.c_str());

  conn->begin_transaction();
  conn->remove(pt);
  conn->commit();

  from_db = conn->query(pt_ptr, hypernate::fields_exclude);

  ASSERT_EQ(from_db.size(), 0);
}

TEST(school_test, teachers) {
  ifstream config("orm.json");
  json configs(config);

  shared_ptr<hypernate::connection> conn(new hypernate::connection(configs));

  shared_ptr<teacher> t(new teacher(conn));
  t->set_value("id", 1);
  auto list = conn->query(t, hypernate::fields_include, {"id"});
  for(auto item : list ) {
    cout << "{ id:" << item->get_value("id") << ", name:" << item->get_value("name") << "}" << endl;
  }

  ASSERT_EQ(list.size(), 1);
  ASSERT_STREQ(list.front()->get_value("name").dump().c_str(), string("\"miss_wang\"").c_str());
}


TEST(one_to_one, teachers_in_classes) {
  ifstream config("orm.json");
  json configs(config);

  shared_ptr<hypernate::connection> conn(new hypernate::connection(configs));

  conn->register_persistent_object(shared_ptr<classes>(new classes(conn)));
  conn->register_persistent_object(shared_ptr<teacher>(new teacher(conn)));
  conn->register_persistent_object(shared_ptr<student>(new student(conn)));
  shared_ptr<classes> search_sample(new classes(conn));
  search_sample->set_value("id", 1);
  auto search_results = conn->query(search_sample, hypernate::fields_include, {"id"});
  auto& searched_class = search_results.front();
  auto search_teacher = searched_class->get_object("chinese_teacher");

  ASSERT_EQ(search_teacher->get_value("id").get<int>(), 1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
