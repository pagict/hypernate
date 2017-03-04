//
// Created by PengPremium on 17/3/4.
//

#ifndef HYPERNATE_CLASSES_H
#define HYPERNATE_CLASSES_H

#include "persistent_object.h"
class classes : public hypernate::persistent_object{
 public:
  const std::string class_name() const {
    return "classes";
  }
};

class teacher  : public hypernate::persistent_object{
 public:
  const std::string class_name() const {
    return "teacher";
  }
};

#endif //HYPERNATE_CLASSES_H
