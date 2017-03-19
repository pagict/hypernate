//
// Created by PengPremium on 17/2/23.
//

#ifndef HYPERNATE_PLAIN_TABLE_H
#define HYPERNATE_PLAIN_TABLE_H

#include "../persistent_object.h"

class plain_table : public hypernate::persistent_object {

  const static std::string _class_name;

 public:
  const std::string class_name() const {
    return _class_name;
  } ;

  plain_table(hypernate::connection* conn)
      : hypernate::persistent_object(_class_name, conn)
  {}

  plain_table(std::shared_ptr<hypernate::connection> conn)
      : hypernate::persistent_object(_class_name, conn)
  {}
};

const std::string plain_table::_class_name = "plain_table";

#endif //HYPERNATE_PLAIN_TABLE_H
