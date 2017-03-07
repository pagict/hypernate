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
};

class classes : public hypernate::persistent_object {
 public:
  const std::string class_name() const {
    return "classes";
  }
};

#endif //HYPERNATE_SCHOOL_H
