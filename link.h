/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include <vector>
#include <list>
#include "Graphs/libgraphs.h"

#ifndef LINK_HEADER_FILE_INCLUDED
#define LINK_HEADER_FILE_INCLUDED

class LIBGRAPHS_API LinkData
{
  void init()
  {
    m_directed = false;
    m_length = 1;
    m_btws = m_clss = 0;
    m_dactTime = -1;
  }

public:
  mutable int m_tag;

  bool m_directed;
  double m_btws;
  double m_clss;
  double m_length;
  int m_dactTime;

  LinkData()
  {
    init();
  }

  LinkData(const LinkData &v) :
    m_directed(v.m_directed), m_length(v.m_length),
    m_btws(0), m_clss(0), m_dactTime(-1)
  {
  }

  const LinkData &operator= (const LinkData &v);

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
