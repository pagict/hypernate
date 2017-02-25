//
// Created by PengPremium on 17/2/23.
//

#ifndef HYPERNATE_PLAIN_TABLE_H
#define HYPERNATE_PLAIN_TABLE_H

#include "persistent_object.h"
class plain_table : public hypernate::persistent_object {

 public:
  const std::string class_name() const {
    return "plain_table";
  } ;
};

#endif //HYPERNATE_PLAIN_TABLE_H
