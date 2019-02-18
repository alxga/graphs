/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/alg.h"


const double EPS0 = 1e-8;


void Alg::CalcDistances(Node *src, const PNodeVector &nodes, bool activeOnly,
                         bool forward)
{
  Alg::CalcDistances(src, &nodes[0], (int)nodes.size(), activeOnly, forward);
}

void Alg::CalcDistances(Node *src, Node * const *nodes, int count,
                        bool activeOnly, bool forward)
{
  for (int j = 0; j < count; j++)
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

    LinkVector &links = forward ? n->links() : n->inLinks();
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


struct HeapItem
{
  Node *node;
  double value;
  bool operator< (const HeapItem &v) const
  {
    return value > v.value;
  }
};

void Alg::CalcDistancesDijkstra(Node *src, Node * const *nodes, int count,
                                bool activeOnly, bool forward)
{
  for (int j = 0; j < count; j++)
    nodes[j]->m_dtag = -1;

  if (activeOnly && src->m_dactTime >= 0)
    return;

  std::priority_queue<HeapItem> heap;

  src->m_dtag = 0;
  HeapItem item = { src, 0 };
  heap.push(item);

  while (heap.size() > 0)
  {
    HeapItem top = heap.top();
    heap.pop();
    if (top.node->m_dtag < top.value)
      continue;

    Node *topNode = top.node;

    LinkVector &links = forward ? topNode->links() : topNode->inLinks();
    for (int i = 0; i < (int)links.size(); i++)
    {
      Node *n2 = links[i].n;
      LinkData *ld = links[i].d;
      double n2Len = topNode->m_dtag + ld->m_length;
      if ((!activeOnly || (n2->m_dactTime < 0 && ld->m_dactTime < 0)) &&
          (n2->m_dtag < 0 || n2Len < n2->m_dtag))
      {
        n2->m_dtag = n2Len;
        HeapItem nItem = { n2, n2Len };
        heap.push(nItem);
      }
    }
  }
}

void Alg::CalcPathTolerance(Node *src, Node *dst, const PNodeVector &nodes,
                            bool activeOnly)
{
  const size_t C = nodes.size();

  Alg::CalcDistances(src, nodes, activeOnly, true);

  if (dst->m_dtag < 0) // no path from src to dst
  {
    for (size_t i = 0; i < C; i++)
      nodes[i]->m_pathTol = -1;
    return;
  }

  for (size_t i = 0; i < C; i++)
  {
    Node &n = *nodes[i];
    n.m_pathTol = n.m_dtag;
  }

  Alg::CalcDistances(dst, nodes, activeOnly, false);

  for (size_t i = 0; i < C; i++)
  {
    Node &n = *nodes[i];
    if (n.m_pathTol < 0 || n.m_dtag < 0) // no path from src or no path to dst
      n.m_pathTol = -1;
    else
      n.m_pathTol = n.m_dtag + n.m_pathTol;
  }
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
  for (int i = 0; i < count; i += step)
  {
    Node *cur = pNodes[i];
    CalcDistances(cur, pNodes, count, activeOnly, true);
    for (int j = 0; j < count; j++)
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
    CalcDistances(cur, pNodes, count, activeOnly, true);

    if (h == 0)
    {
      for (int i = 0; i < count; i++)
        if (pNodes[i]->m_dtag < 0)
          throw Exception("The graph/component is not connected");
    }

    Node *nMax;
    double max = -1;    
    for (int i = 0; i < count; i++)
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

  for (int i = 0; i < count; i++)
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

  for (int i = 0; i < count; i++)
  {
    Node *cur = pNodes[i];
    CalcDistances(cur, pNodes, count, activeOnly, true);

    for (int j = 0; j < count; j++)
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
  for (int i = 0; i < count; i++)
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

int Alg::AssignUComponentIDs(const PNodeVector &nodes, bool activeOnly)
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
  int ret = components.size() > 0 ? (int)components[0]->size() : -1;

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

  return ret;
}

int Alg::AssignWkComponentIDs(const PNodeVector &nodes, bool activeOnly)
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
  int ret = components.size() > 0 ? (int)components[0]->size() : -1;

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

  return ret;
}

int Alg::AssignSgComponentIDs(const PNodeVector &nodes)
{
  for (size_t i = 0; i < nodes.size(); i++)
    nodes[i]->m_tag = -1;
  
  // Run backward DFS
  // Node tags: -1: unseen; 0: added to the stack; 1: in-links processed
  PNodeList L;
  PNodeStack S;
  for (size_t i = 0; i < nodes.size(); i++)
  {
    Node &n = *nodes[i];
    if (n.m_tag > 0)
      continue;

    S.push(&n);
    n.m_tag = 0;

    while (S.size() > 0)
    {
      Node *sn = S.top();
      if (sn->m_tag > 0)
      {
        S.pop();
        L.push_front(sn);
      }
      else
      {
        const LinkVector &iLinks = sn->inLinks();
        for (size_t j = 0; j < iLinks.size(); j++)
        {
          Node *src = iLinks[j].n;
          if (src->m_tag < 0)
          {
            S.push(src);
            src->m_tag = 0;
          }
        }
        sn->m_tag = 1;
      }
    }
  }

  // Run forward BFS
  // Node tags: 1: unseen; 2: processed
  PNodeQueue Q;
  std::map<Node *, PNodeList *> D;
  for (PNodeList::iterator it = L.begin(); it != L.end(); it++)
  {
    Node *n = *it;
    if (n->m_tag > 1)
      continue;

    PNodeList *l = new PNodeList();
    D[n] = l;

    Q.push(n);
    n->m_tag = 2;
    l->push_back(n);

    while (Q.size() > 0)
    {
      Node *sn = Q.front();
      Q.pop();
      const LinkVector &links = sn->links();
      for (size_t j = 0; j < links.size(); j++)
      {
        Node *dst = links[j].n;
        if (dst->m_tag <= 1)
        {
          Q.push(dst);
          dst->m_tag = 2;
          l->push_back(dst);
        }
      }
    }
  }

  std::vector<PNodeList *> components;
  for (std::map<Node *, PNodeList *>::iterator mit = D.begin();
       mit != D.end(); mit++)
    components.push_back(mit->second);
  std::sort(components.begin(), components.end(), sg_compPNodeListsBySize);
  int ret = components.size() > 0 ? (int)components[0]->size() : -1;

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

  return ret;
}
