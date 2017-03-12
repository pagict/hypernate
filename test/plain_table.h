//
// Created by PengPremium on 17/2/23.
//

#ifndef HYPERNATE_PLAIN_TABLE_H
#define HYPERNATE_PLAIN_TABLE_H

#include "../persistent_object.h"

class plain_table : public hypernate::persistent_object {

 public:
  const std::string class_name() const {
    return "plain_table";
  } ;

  plain_table(hypernate::connection* conn)
      : hypernate::persistent_object(conn)
  {}
  plain_table(std::shared_ptr<hypernate::connection> conn)
      : hypernate::persistent_object(conn)
  {}
};

#endif //HYPERNATE_PLAIN_TABLE_H
