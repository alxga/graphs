/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include <map>
#include <queue>
#include <stack>
#include "Utils/dpoint.h"
#include "Graphs/libgraphs.h"
#include "Graphs/link.h"

#ifndef NODE_HEADER_FILE_INCLUDED
#define NODE_HEADER_FILE_INCLUDED

class Node;


typedef std::list<Node *> PNodeList;
typedef std::vector<Node *> PNodeVector;
typedef std::queue<Node *> PNodeQueue;
typedef std::stack<Node *> PNodeStack;

typedef std::map<int, PNodeVector *> IntPNodeVectorMap;
typedef std::map<int, PNodeVector *>::iterator IntPNodeVectorMapIt;
typedef std::map<int, PNodeVector *>::reverse_iterator IntPNodeVectorMapRIt;

typedef std::map<std::string, Node *> StrPNodeMap;
typedef std::map<std::string, Node *>::iterator StrPNodeMapIt;
typedef std::map<std::string, Node *>::reverse_iterator StrPNodeMapRIt;

class Node
{
private:
  LinkVector m_links;
  LinkVector m_inLinks;

  void initLinks()
  {
    m_links.clear();
    m_linksMap = NULL;
    m_inLinks.clear();
  }

  void init()
  {
    m_name = "";
    m_compId = -1;
    m_timesInGC = 0;
    m_timesOnPath = 0;
    m_compSize = 0;
    m_annd = m_btws = m_clss = m_clCoef = 0;
    m_dactTime = -1;
    m_pathTol = 0;

    initLinks();
  }

public:
  std::string m_name;
  mutable int m_tag;
  mutable double m_dtag;
  mutable Node *m_ntag;

  int m_compId;
  size_t m_compSize;
  int m_dactTime;
  int m_numActLinks;

  int m_timesInGC;
  int m_timesOnPath;
  double m_pathTol;

  double m_annd;
  double m_btws;
  double m_clss;
  double m_clCoef;

  std::map<Node *, LinkData *> *m_linksMap;

  Node()
  {
    init();
  }

  virtual ~Node()
  {
    delete m_linksMap;
  }

  virtual void reinit()
  {
    delete m_linksMap;
    init();
  }

  virtual void reinitLinks()
  {
    delete m_linksMap;
    initLinks();
  }

  void link(Node *n, LinkData *d)
  {
    Link l = { n, d };
    if (m_links.size() == 300)
    {
      m_linksMap = new std::map<Node *, LinkData *>();
      for (size_t i = 0; i < m_links.size(); i++)
        (*m_linksMap)[m_links[i].n] = m_links[i].d;
    }
    m_links.push_back(l);
    if (m_linksMap != NULL)
      (*m_linksMap)[n] = d;

    Link il = { this, d };
    n->m_inLinks.push_back(il);
  }

  void updateNumActLinks()
  {
    m_numActLinks = 0;
    for (size_t i = 0; i < m_links.size(); i++)
      if (m_links[i].d->m_dactTime < 0)
        m_numActLinks ++;
  }
  inline int numLinks() const { return (int)m_links.size(); }
  inline int numInLinks() const { return (int)m_inLinks.size(); }
  inline int numActLinks() const { return (int)m_numActLinks; }
  
  inline LinkVector &links() { return m_links; }
  inline LinkVector &inLinks() { return m_inLinks; }

  void getNeighbors(std::map<Node *, bool> &ret) const
  {
    for (size_t i = 0; i < m_links.size(); i++)
      ret[m_links[i].n] = true;
    for (size_t i = 0; i < m_inLinks.size(); i++)
      ret[m_inLinks[i].n] = true;
  }

  inline LinkData *findLink(Node *n2)
  {
    if (m_linksMap != NULL)
    {
      std::map<Node *, LinkData *>::iterator it = m_linksMap->find(n2);
      if (it == m_linksMap->end())
        return NULL;
      else
        return it->second;
    }
    for (size_t i = 0; i < m_links.size(); i++)
      if (m_links[i].n == n2)
        return m_links[i].d;
    return NULL;
  }

  inline LinkData *findInLink(Node *n1)
  {
    for (size_t i = 0; i < m_inLinks.size(); i++)
      if (m_inLinks[i].n == n1)
        return m_inLinks[i].d;
    return NULL;
  }

    
  DPoint m_coords;
  void setLng(double lng)
  {
    m_coords.setLng(lng);
  }
  void setLat(double lat)
  {
    m_coords.setLat(lat);
  }
  inline double lat() const
  {
    return m_coords.lat();
  }
  inline double lng() const
  {
    return m_coords.lng();
  }
};


typedef std::list<Node> NodeList;
typedef std::vector<Node> NodeVector;

typedef std::queue<Node> NodeQueue;
typedef std::stack<Node> NodeStack;

#endif // NODE_HEADER_FILE_INCLUDED
