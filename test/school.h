//
// Created by PengPremium on 17/3/5.
//

#ifndef HYPERNATE_SCHOOL_H
#define HYPERNATE_SCHOOL_H

#include "../persistent_object.h"
class teacher : public hypernate::persistent_object {
 public:
  const std::string class_name() const {
    return "teacher";
  }
  teacher(hypernate::connection* conn)
      : hypernate::persistent_object(conn)
  {}
  teacher(std::shared_ptr<hypernate::connection> conn)
      : hypernate::persistent_object(conn)
  {}
};

class classes : public hypernate::persistent_object {
 public:
  const std::string class_name() const {
    return "classes";
  }
  classes(hypernate::connection* conn)
  : hypernate::persistent_object(conn)
      {}
  classes(std::shared_ptr<hypernate::connection> conn)
  : hypernate::persistent_object(conn)
      {}
};

#endif //HYPERNATE_SCHOOL_H
