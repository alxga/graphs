/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Graphs/node.h"

#ifndef BNODE_HEADER_FILE_INCLUDED
#define BNODE_HEADER_FILE_INCLUDED

class BNode;

class BNode : public Node
{
  BNode *m_dep;
  BNode *m_ctrl;

public:
  BNode()
  {
    m_dep = m_ctrl = NULL;
  }
  
  inline BNode *ctrl() const { return m_ctrl; }
  inline BNode *dep() const { return m_dep; }

  void setDep(BNode *n2)
  {
    m_dep = n2;
    n2->m_ctrl = this;
  }

  inline bool controlOk()
  {
    return m_ctrl == NULL || m_ctrl->m_dactTime < 0;
  }
};

#endif // BNODE_HEADER_FILE_INCLUDED
