//
// Created by PengPremium on 17/3/5.
//

#ifndef HYPERNATE_SCHOOL_H
#define HYPERNATE_SCHOOL_H

#include "../persistent_object.h"
class teacher : public hypernate::persistent_object {
 public:
  const static std::string _class_name;

  const std::string class_name() const {
    return _class_name;
  }
  teacher(const std::string&, hypernate::connection* conn = nullptr)
      : hypernate::persistent_object(_class_name, conn)
  {}
  teacher(std::shared_ptr<hypernate::connection> conn)
      : hypernate::persistent_object(_class_name, conn)
  {}
};

const std::string teacher::_class_name = "teacher";

class classes : public hypernate::persistent_object {
 public:
  const static std::string _class_name;

  const std::string class_name() const {
    return _class_name;
  }
  classes(const std::string&, hypernate::connection* conn = nullptr)
  : hypernate::persistent_object(_class_name, conn)
      {}
  classes(std::shared_ptr<hypernate::connection> conn)
  : hypernate::persistent_object(_class_name, conn)
      {}
};

class student: public hypernate::persistent_object {
 public:
  const static std::string _class_name;

  const std::string class_name() const {
    return _class_name;
  }
  student(const std::string&, hypernate::connection* conn = nullptr)
      : hypernate::persistent_object(_class_name, conn)
  {}
  student(std::shared_ptr<hypernate::connection> conn)
      : hypernate::persistent_object(_class_name, conn)
  {}
};

const std::string classes::_class_name = "classes";
const std::string student::_class_name = "student";

#endif //HYPERNATE_SCHOOL_H
