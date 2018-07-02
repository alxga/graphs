/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/alg.h"


const double EPS0 = 1e-8;


void Alg::CalcUDistances(Node *src, const PNodeVector &nodes, bool activeOnly)
{
  Alg::CalcUDistances(src, &nodes[0], (int)nodes.size(), activeOnly);
}

void Alg::CalcUDistances(Node *src, Node * const *nodes, int count,
                         bool activeOnly)
{
  for (size_t j = 0; j < count; j++)
    nodes[j]->m_dtag = -1;

  if (activeOnly && src->m_dactTime >= 0)
    return;

  src->m_dtag = 0;
  src->m_ntag = NULL;

  double max = 0;
  Node *nMax = src;

  PNodeList queue;
  queue.push_back(src);
  while (!queue.empty())
  {
    Node *n = queue.front();
    queue.pop_front();
    double curd = n->m_dtag;

    LinkVector &links = n->links();
    for (size_t j = 0; j < links.size(); j++)
    {
      LinkData *l = links[j].d;
      Node *n2 = links[j].n;
      if ((!activeOnly || (l->m_dactTime < 0 && n2->m_dactTime < 0)) &&
          (n2->m_dtag < 0 || n2->m_dtag > curd + l->m_length))
      {
        n2->m_dtag = curd + l->m_length;
        n2->m_ntag = n;
        queue.push_back(n2);
      }
    }
  }
}


int Alg::CalcNodesOnUPath(Node *src, Node *dst, const PNodeVector &nodes,
                          bool activeOnly)
{
  for (size_t i = 0; i < nodes.size(); i++)
    nodes[i]->m_tag = 0;

  int ret = 0;

  Alg::CalcUDistances(src, nodes, activeOnly);
  if (dst->m_dtag < 0) // no path from src to dst
    return 0;

  PNodeList dq;

  dq.push_back(dst);
  dst->m_tag = 1;
  ret++;

  while (!dq.empty())
  {
    Node *t = dq.front();
    dq.pop_front();

    // we assume link lengths above zero
    const LinkVector &iLinks = t->inLinks();
    for (size_t i = 0; i < iLinks.size(); i++)
    {
      Node *f = iLinks[i].n;
      LinkData *ld = iLinks[i].d;
      // check that there is a path, node hasn't been visited yet, and
      // path length from the node is short enough
      if (f->m_dtag >= 0 && f->m_tag == 0 &&
          std::abs(f->m_dtag + ld->m_length - t->m_dtag) < EPS0)
      {
        dq.push_back(f);
        f->m_tag = 1;
        ret++;
      }
    }
  }

  return ret;
}


#define INIT_PNODES_COMPID \
  int count = 0; \
  size_t fCount = nodes.size(); \
  Node **pNodes = new Node *[fCount]; \
  auto_del<Node *> del_pNodes(pNodes, true); \
  for (size_t i = 0; i < fCount; i++) \
    if (compId < 0 || compId == nodes[i]->m_compId) \
    { \
      pNodes[count++] = nodes[i]; \
      nodes[i]->m_tag = 0; \
    }


double Alg::ApproxUAvClss(const PNodeVector &nodes, bool activeOnly,
                          int compId)
{
  INIT_PNODES_COMPID

  int n = 0;
  double sum = 0;
  int step = count > 1000 ? count / 1000 : 1;
  for (size_t i = 0; i < count; i += step)
  {
    Node *cur = pNodes[i];
    CalcUDistances(cur, pNodes, count, activeOnly);
    for (size_t j = 0; j < count; j++)
    {
      Node &nd = *pNodes[j];
      if (nd.m_dtag < 0)
        throw Exception("The graph/component is not connected");
      sum += pNodes[j]->m_dtag;
    }
    n ++;
  }
  
  return sum / (n * count);
}

double Alg::ApproxUDiameter(const PNodeVector &nodes, bool activeOnly,
                            int compId)
{
  INIT_PNODES_COMPID

  double D = -1;
  Node *cur = pNodes[0];
  cur->m_tag = 1;
  size_t hopsCount = 20;
  for (size_t h = 0; h < hopsCount; h++)
  {
    CalcUDistances(cur, pNodes, count, activeOnly);

    if (h == 0)
    {
      for (size_t i = 0; i < count; i++)
        if (pNodes[i]->m_dtag < 0)
          throw Exception("The graph/component is not connected");
    }

    Node *nMax;
    double max = -1;    
    for (size_t i = 0; i < count; i++)
      if (pNodes[i]->m_dtag > max && pNodes[i]->m_tag == 0)
      {
        nMax = pNodes[i];
        max = nMax->m_dtag;
      }
    if (max > D)
      D = max;
    if (max < 0) // graph size is smaller than hopsCount
      break;
    nMax->m_tag = 1;
    cur = nMax;
  }
  return D;
}

void Alg::CalcUCentralities(const PNodeVector &nodes, bool activeOnly,
                            double *D, int compId)
{
  INIT_PNODES_COMPID

  double d = 0;

  for (size_t i = 0; i < count; i++)
  {
    Node *n = pNodes[i];
    n->m_btws = n->m_clss = 0;
    LinkVector &links = n->links();
    for (size_t j = 0; j < links.size(); j++)
    {
      LinkData *l = links[j].d;
      l->m_btws = l->m_clss = 0;
    }
  }

  for (size_t i = 0; i < count; i++)
  {
    Node *cur = pNodes[i];
    CalcUDistances(cur, pNodes, count, activeOnly);

    for (size_t j = 0; j < count; j++)
    {
      Node *n2 = pNodes[j];
      if (cur == n2)
        continue;
      if (n2->m_dtag > d)
        d = n2->m_dtag;
      cur->m_clss += n2->m_dtag;

      Node *dst = n2;
      if (dst->m_dtag >= 0)
      {
        Node *src = dst->m_ntag;
        while (src != NULL)
        {
          LinkData *l = src->findLink(dst);
          l->m_btws++;
          if (src != cur)
            src->m_btws++;
          dst = src;
          src = dst->m_ntag;
        }
      }
      else
        throw Exception("The graph/component is not connected");
    }
    cur->m_clss /= count;
  }
  for (size_t i = 0; i < count; i++)
  {
    Node *src = pNodes[i];
    LinkVector &links = src->links();
    for (size_t j = 0; j < links.size(); j++)
    {
      LinkData *l = links[j].d;
      Node *dst = links[j].n;
      l->m_clss = (src->m_clss + dst->m_clss) / 2;
    }
  }
  if (D != NULL)
    *D = d;
}


void Alg::CalcUAvNNDegree(const PNodeVector &nodes)
{
  const size_t count = nodes.size();
  for (size_t i = 0; i < count; i++)
  {
    Node &n = *nodes[i];
    n.m_tag = (int)n.numLinks();
  }
  for (size_t i = 0; i < count; i++)
  {
    Node &n = *nodes[i];
    std::map<Node *, bool> ret;
    n.getNeighbors(ret);
    n.m_annd = 0;
    if (ret.size() > 0)
    {
      for (std::map<Node *, bool>::iterator it = ret.begin();
           it != ret.end(); it++)
        n.m_annd += it->first->m_tag;
      n.m_annd /= ret.size();
    }
  }
}


void Alg::CalcUClustering(const PNodeVector &nodes)
{
  const int count = (int)nodes.size();
  for (int i = 0; i < count; i++)
    nodes[i]->m_tag = i;
  for (int i = 0; i < count; i++)
  {
    Node &n = *nodes[i];
    std::map<Node *, bool> ret;
    n.getNeighbors(ret);

    int cl = 0;
    for (std::map<Node *, bool>::iterator it = ret.begin();
         it != ret.end(); it++)
    {
      Node *n1 = it->first;
      const LinkVector &ls = n1->links();
      for (size_t j = 0; j < ls.size(); j++)
      {
        Node *n2 = ls[j].n;
        if (n1->m_tag < n2->m_tag && ret.find(n2) != ret.end())
          cl++;
      }
    }

    if (ret.size() > 1)
      n.m_clCoef = 2.0 * cl / (ret.size() * (ret.size() - 1));
    else
      n.m_clCoef = 0;
  }
}


static bool sg_compPNodeListsBySize(const PNodeList *list1,
                                    const PNodeList *list2)
{
  return list2->size() < list1->size();
}

// macro is to simplify weak components calculation in directed networks
#define PROCESS_CONNCOMP_LINKS(_links, _linksSize) \
  for (int i = 0; i < (_linksSize); i++) \
  { \
    if (activeOnly && (_links)[i].d->m_dactTime >= 0) \
      continue; \
    Node *n2 = (_links)[i].n; \
    if (n2->m_tag == 0) \
    { \
      queue.push_back(n2); \
      n2->m_tag = 1; \
    } \
  }

void Alg::AssignUComponentIDs(const PNodeVector &nodes, bool activeOnly)
{
  const size_t count = nodes.size();
  for (size_t i = 0; i < count; i++)
  {
    Node &n = *nodes[i];
    if (!activeOnly || n.m_dactTime < 0)
      n.m_tag = 0;
    else
      n.m_tag = n.m_compId = -1;
  }

  std::vector<PNodeList *> components;
  PNodeList queue;

  for (size_t i = 0; i < count; i++)
  {
    Node *n = nodes[i];
    if (n->m_tag != 0)
      continue;

    queue.push_back(n);
    n->m_tag = 1;

    PNodeList *current = new PNodeList();

    while (!queue.empty())
    {
      Node *qn = queue.front();
      queue.pop_front();

      current->push_back(qn);

      const LinkVector &links = qn->links();
      const int linksSize = (int)links.size();
      // add unvisited, active (tag == 0) nodes to the queue,
      // and assign their tag to 1
      PROCESS_CONNCOMP_LINKS(links, linksSize)
    }

    components.push_back(current);
  }
  
  std::sort(components.begin(), components.end(), sg_compPNodeListsBySize);

  for (size_t i = 0; i < components.size(); i++)
  {
    PNodeList *lst = components[i];
    for (PNodeList::iterator lit = lst->begin(); lit != lst->end(); lit++)
    {
      (*lit)->m_compId = (int)i;
      (*lit)->m_compSize = lst->size();
    }
    delete lst;
  }
}

void Alg::AssignWkComponentIDs(const PNodeVector &nodes, bool activeOnly)
{
  const size_t count = nodes.size();
  for (size_t i = 0; i < count; i++)
  {
    Node &n = *nodes[i];
    if (!activeOnly || n.m_dactTime < 0)
      n.m_tag = 0;
    else
      n.m_tag = n.m_compId = -1;
  }

  std::vector<PNodeList *> components;
  PNodeList queue;

  for (size_t i = 0; i < count; i++)
  {
    Node *n = nodes[i];
    if (n->m_tag != 0)
      continue;

    queue.push_back(n);
    n->m_tag = 1;

    PNodeList *current = new PNodeList();

    while (!queue.empty())
    {
      Node *qn = queue.front();
      queue.pop_front();

      current->push_back(qn);

      const LinkVector &links = qn->links();
      const int linksSize = (int)links.size();
      // add unvisited, active (tag == 0) nodes to the queue,
      // and assign their tag to 1
      PROCESS_CONNCOMP_LINKS(links, linksSize)

      const LinkVector &inLinks = qn->inLinks();
      const int inLinksSize = (int)inLinks.size();
      // add unvisited, active (tag == 0) nodes to the queue,
      // and assign their tag to 1
      PROCESS_CONNCOMP_LINKS(inLinks, inLinksSize)
    }

    components.push_back(current);
  }

  std::sort(components.begin(), components.end(), sg_compPNodeListsBySize);

  for (size_t i = 0; i < components.size(); i++)
  {
    PNodeList *lst = components[i];
    for (PNodeList::iterator lit = lst->begin(); lit != lst->end(); lit++)
    {
      (*lit)->m_compId = (int)i;
      (*lit)->m_compSize = lst->size();
    }
    delete lst;
  }
}

void Alg::AssignSgComponentIDs(const PNodeVector &nodes)
{
  for (size_t i = 0; i < nodes.size(); i++)
    nodes[i]->m_tag = -1;

  PNodeList L;
  PNodeStack S;
  for (size_t i = 0; i < nodes.size(); i++)
  {
    Node &n = *nodes[i];
    if (n.m_tag >= 0)
      continue;
    S.push(&n);
    while (S.size() > 0)
    {
      Node *sn = S.top();
      if (sn->m_tag >= 0)
      {
        S.pop();
        L.push_front(sn);
      }
      else
      {
        const LinkVector &oLinks = sn->links();
        for (size_t j = 0; j < oLinks.size(); j++)
          if (oLinks[j].n->m_tag < 0)
            S.push(oLinks[j].n);
        sn->m_tag = 0;
      }
    }
  }

  std::map<Node *, PNodeList *> D;
  for (PNodeList::iterator it = L.begin(); it != L.end(); it++)
  {
    Node *n = *it;
    if (n->m_tag > 0)
      continue;
    PNodeList *l = new PNodeList();
    D[n] = l;
    S.push(n);
    while (S.size() > 0)
    {
      Node *sn = S.top();
      S.pop();
      if (sn->m_tag <= 0)
      {
        const LinkVector &iLinks = sn->inLinks();
        for (size_t j = 0; j < iLinks.size(); j++)
          if (iLinks[j].n->m_tag > 0)
            S.push(iLinks[j].n);
        sn->m_tag = 1;
        l->push_back(sn);
      }
    }
  }

  std::vector<PNodeList *> components;
  for (std::map<Node *, PNodeList *>::iterator mit = D.begin();
       mit != D.end(); mit++)
    components.push_back(mit->second);
  std::sort(components.begin(), components.end(), sg_compPNodeListsBySize);

  for (size_t i = 0; i < components.size(); i++)
  {
    PNodeList *lst = components[i];
    for (PNodeList::iterator lit = lst->begin(); lit != lst->end(); lit++)
    {
      (*lit)->m_compId = (int)i;
      (*lit)->m_compSize = lst->size();
    }
    delete lst;
  }
}
