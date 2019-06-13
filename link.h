/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include <vector>
#include <list>
#include "Graphs/libgraphs.h"

#ifndef LINK_HEADER_FILE_INCLUDED
#define LINK_HEADER_FILE_INCLUDED

class LinkData
{
  void init()
  {
    m_name = "";
    m_directed = false;
    m_weight = m_length = 1;
    m_btws = -1;
    m_dactTime = -1;
    m_isTemp = false;
  }

public:
  mutable int m_tag;
  mutable double m_dtag;

  std::string m_name;
  bool m_directed;
  double m_btws;
  double m_length;
  double m_weight;
  int m_dactTime;
  bool m_isTemp;

  LinkData()
  {
    init();
  }

  const LinkData &operator= (const LinkData &v)
  {
    memcpy(this, &v, sizeof(LinkData));
    return *this;
  }

  virtual void reinit()
  {
    init();
  }
};
typedef std::list<LinkData> LinkDataList;
typedef std::vector<LinkData> LinkDataVector;
typedef std::list<LinkData *> PLinkDataList;
typedef std::vector<LinkData *> PLinkDataVector;


class Node;
struct Link
{
  Node *n;
  LinkData *d;
};
typedef std::list<Link> LinkList;
typedef std::vector<Link> LinkVector;
typedef std::list<Link *> PLinkList;
typedef std::vector<Link *> PLinkVector;

struct SrcLink
{
  Node *src;
  Link link;
};
typedef std::list<SrcLink> SrcLinkList;
typedef std::vector<SrcLink> SrcLinkVector;
typedef std::list<SrcLink *> PSrcLinkList;
typedef std::vector<SrcLink *> PSrcLinkVector;


#endif // LINK_HEADER_FILE_INCLUDED
