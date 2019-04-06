/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/alg.h"


const double EPS0 = 1e-8;


double Alg::INF = 1e300;


void Alg::CalcDistancesBFS(Node *src, const PNodeVector &nodes,
                           bool activeOnly, bool forward)
{
  Alg::CalcDistancesBFS(src, &nodes[0], (int)nodes.size(), activeOnly,
                        forward);
}

void Alg::CalcDistancesBFS(Node *src, Node * const *nodes, int count,
                           bool activeOnly, bool forward)
{
#if !defined(_DEBUG) && _MSC_VER >= 1600 && _MSC_VER < 1700
  // queue implementation appears to be much slower in release mode
  // than the priority queue implementation as tested in MS VS 2010
  Alg::RunDijkstra(src, nodes, count, activeOnly, forward);
#else
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
      if (activeOnly && (l->m_dactTime >= 0 || n2->m_dactTime >= 0))
        continue;
      if (n2->m_dtag < 0)
      {
        n2->m_dtag = curd + l->m_length;
        n2->m_ntag = n;
        queue.push_back(n2);
      }
      else if (n2->m_dtag > curd + l->m_length)
        throw Exception("Unsupported link lengths detected in a BFS paths "
                        "calculation");
    }
  }
#endif
}

void Alg::CalcPathToleranceBFS(Node *src, Node *dst,
                               const PNodeVector &nodes,
                               bool activeOnly)
{
  const size_t C = nodes.size();

  Alg::CalcDistancesBFS(src, nodes, activeOnly, true);

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

  Alg::CalcDistancesBFS(dst, nodes, activeOnly, false);

  for (size_t i = 0; i < C; i++)
  {
    Node &n = *nodes[i];
    if (n.m_pathTol < 0 || n.m_dtag < 0) // no path from src or no path to dst
      n.m_pathTol = -1;
    else
      n.m_pathTol = n.m_dtag + n.m_pathTol;
  }
}


double Alg::ApproxAvClssBFS(const PNodeVector &nodes, bool activeOnly)
{
  int n = 0;
  double sum = 0;
  int count = (int)nodes.size();
  int step = count > 1000 ? count / 1000 : 1;
  for (int i = 0; i < count; i += step)
  {
    Node &cur = *nodes[i];

    if (activeOnly && cur.m_dactTime >= 0)
      continue;

    CalcDistancesBFS(&cur, &nodes[0], count, activeOnly, false);
    for (int j = 0; j < count; j++)
    {
      Node &nd = *nodes[j];
      if (nd.m_dtag > 0)
        sum += 1.0 / nd.m_dtag;
    }
    n ++;
  }  
  return sum / (n * count);
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

double Alg::ApproxAvFarnessBFS(const PNodeVector &nodes, bool activeOnly,
                               int compId)
{
  INIT_PNODES_COMPID

  int n = 0;
  double sum = 0;
  int step = count > 1000 ? count / 1000 : 1;
  for (int i = 0; i < count; i += step)
  {
    Node *cur = pNodes[i];
    CalcDistancesBFS(cur, pNodes, count, activeOnly, false);
    for (int j = 0; j < count; j++)
    {
      Node &nd = *pNodes[j];
      if (nd.m_dtag < 0)
        throw Exception("The graph/component is not strongly connected");
      sum += pNodes[j]->m_dtag;
    }
    n ++;
  }

  return sum / (n * count);
}

double Alg::ApproxUDiameterBFS(const PNodeVector &nodes, bool activeOnly,
                               int compId)
{
  INIT_PNODES_COMPID

  double D = -1;
  Node *cur = pNodes[0];
  cur->m_tag = 1;
  size_t hopsCount = 20;
  for (size_t h = 0; h < hopsCount; h++)
  {
    CalcDistancesBFS(cur, pNodes, count, activeOnly, true);

    if (h == 0)
    {
      for (int i = 0; i < count; i++)
        if (pNodes[i]->m_dtag < 0)
          throw Exception("The graph/component is not strongly connected");
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


void Alg::CalcCentralitiesBFS(const PNodeVector &nodes, bool activeOnly,
                              double *diam)
{
  // if activeOnly is true:
  //  - set all inactive nodes centralities to -1
  //  - set all inactive links centralities to -1
  //  - set all inactive nodes incident link centralities to -1
  // copy relevant nodes to pNodes and save the number of active nodes as count
  // initialize relevant nodes and links centralities to 0
  int count = 0;
  size_t fCount = nodes.size();
  Node **pNodes = new Node *[fCount];
  auto_del<Node *> del_pNodes(pNodes, true);
  for (size_t i = 0; i < fCount; i++)
  {
    Node *n = nodes[i];
    LinkVector &links = n->links();
    LinkVector &inLinks = n->inLinks();
    if (activeOnly && n->m_dactTime >= 0)
    {
      n->m_btws = n->m_clss = n->m_frns = -1;
      for (size_t j = 0; j < links.size(); j++)
        links[j].d->m_btws = -1;
      for (size_t j = 0; j < inLinks.size(); j++)
        links[j].d->m_btws = -1;
    }
    else
    {
      n->m_btws = n->m_clss = n->m_frns = 0;
      pNodes[count++] = nodes[i];
      for (size_t j = 0; j < links.size(); j++)
      {
        LinkData *ld = links[j].d;
        if (activeOnly && ld->m_dactTime >= 0)
          ld->m_btws = -1;
        else
          ld->m_btws = 0;
      }
    }
  }


  double d = 0; // temporary variable to store the diameter
  for (int i = 0; i < count; i++)
  {
    Node *cur = pNodes[i];
    CalcDistancesBFS(cur, pNodes, count, activeOnly, false);

    int ccSize = 1;
    for (int j = 0; j < count; j++)
    {
      Node *src = pNodes[j];
      if (cur == src || src->m_dtag < 0)
        continue;

      if (src->m_dtag > d)
        d = src->m_dtag;

      cur->m_clss += 1.0 / src->m_dtag;
      cur->m_frns += src->m_dtag;
      ccSize++;

      Node *prv = src;
      Node *nxt = prv->m_ntag;
      while (nxt != cur)
      {
        LinkData *ld = prv->findLink(nxt);
        ld->m_btws++;
        nxt->m_btws++;
        prv = nxt;
        nxt = prv->m_ntag;
      }
      prv->findLink(nxt)->m_btws++;
    }

    cur->m_clss /= fCount;
    cur->m_frns /= ccSize;
  }
  if (diam != NULL)
    *diam = d;
}


struct PQItemDijkstra
{
  Node *node;
  double value;
  bool operator< (const PQItemDijkstra &v) const
  {
    return value > v.value;
  }
};

void Alg::RunDijkstra(Node *src, Node * const *nodes, int count,
                      bool activeOnly, bool forward)
{
  for (int j = 0; j < count; j++)
    nodes[j]->m_dtag = -1;

  if (activeOnly && src->m_dactTime >= 0)
    return;

  std::priority_queue<PQItemDijkstra> heap;

  src->m_dtag = 0;
  PQItemDijkstra item = { src, 0 };
  heap.push(item);

  while (heap.size() > 0)
  {
    PQItemDijkstra top = heap.top();
    heap.pop();
    if (top.node->m_dtag < top.value)
      continue;

    Node *topNode = top.node;

    LinkVector &links = forward ? topNode->links() : topNode->inLinks();
    for (int i = 0; i < (int)links.size(); i++)
    {
      Node *n2 = links[i].n;
      LinkData *ld = links[i].d;
      if (activeOnly && (ld->m_dactTime >= 0 || n2->m_dactTime >= 0))
        continue;

      double n2Len = topNode->m_dtag + ld->m_length;

      if (n2->m_dtag < 0 || n2Len < n2->m_dtag)
      {
        n2->m_dtag = n2Len;
        PQItemDijkstra nItem = { n2, n2Len };
        heap.push(nItem);
      }
    }
  }
}

void Alg::RunDijkstra(Node *src, const PNodeVector &nodes, bool activeOnly,
                      bool forward)
{
  Alg::RunDijkstra(src, &nodes[0], (int)nodes.size(), activeOnly, forward);
}


void Alg::RunBellmanFord(Node *src, Node * const *nodes, int count)
{
  double *a = new double [2 * count];
  auto_del<double> del_a(a, true);

  for (int i = 0; i < count; i++)
    nodes[i]->m_tag = i;
  if (src != NULL)
  {
    for (int i = 0; i < count; i++)
      a[i] = INF;
    a[src->m_tag] = 0;
    src->m_ntag = NULL;
  }
  else
  {
    for (int i = 0; i < count; i++)
      nodes[i]->m_ntag = NULL;
    memset(a, 0, count * sizeof(double));
  }

  bool updated;
  int prevOffs, curOffs;
  for (int s = 1; s <= count; s++)
  {
    prevOffs = ((s + 1) % 2) * count;
    curOffs = (s % 2) * count;
    updated = false;
    for (int j = 0; j < count; j++)
    {
      Node *n = nodes[j];
      const LinkVector &inLinks = n->inLinks();
      double bestLength = a[j + prevOffs];
      Node *bestFrom = NULL;
      for (int k = 0; k < (int)inLinks.size(); k++)
      {
        Node *from = inLinks[k].n;
        double length = inLinks[k].d->m_length +
                        a[from->m_tag + prevOffs];
        if (length < bestLength)
        {
          bestLength = length;
          bestFrom = from;
        }
      }
      if (bestFrom != NULL)
      {
        updated = true;
        n->m_ntag = bestFrom;
      }
      a[j + curOffs] = bestLength;
    }
    if (!updated)
      break;
  }
  if (updated)
    throw Exception("A negative loop detected in Bellman-Ford");

  for (int i = 0; i < count; i++)
    nodes[i]->m_dtag = a[i + curOffs];
}

void Alg::RunBellmanFord(Node *src, const PNodeVector &nodes)
{
  Alg::RunBellmanFord(src, &nodes[0], (int)nodes.size());
}


static void sg_PrintFWMatrix(double *a, int c)
{
  if (c > 10)
  {
    printf("The matrix size is more than 10" ENDL);
    return;
  }
  for (int i = 0; i < c; i++)
  {
    if (a[i * c] < Alg::INF)
      printf("%3.0lf", a[i * c]);
    else
      printf("INF");
    for (int j = 1; j < c; j++)
      if (a[i * c + j] < Alg::INF)
        printf(" %3.0lf", a[i * c + j]);
      else
        printf(" INF");
    printf(ENDL);
  }
}

double Alg::RunFloydWarshall(Node * const *nodes, int count)
{
  double *a1 = new double [count * count];
  auto_del<double> del_a1(a1, true);
  double *a2 = new double [count * count];
  auto_del<double> del_a2(a2, true);

  for (int i = 0; i < count; i++)
    nodes[i]->m_tag = i;
  
  for (int i = 0; i < count * count; i++)
    a1[i] = INF;
  for (int i = 0; i < count; i++)
  {
    a1[i + i * count] = 0;
    const LinkVector &links = nodes[i]->links();
    for (int j = 0; j < (int)links.size(); j++)
      a1[i * count + links[j].n->m_tag] = links[j].d->m_length;
  }

  //printf("Initial Matrix:" ENDL);
  //sg_PrintFWMatrix(a1, count);

  double *prevA, *curA;
  for (int k = 0; k < count; k++)
  {
    if (k % 2 == 0)
    {
      prevA = a1;
      curA = a2;
    }
    else
    {
      prevA = a2;
      curA = a1;
    }

    for (int i = 0; i < count; i++)
      for (int j = 0; j < count; j++)
      {
        double prevLength = prevA[i * count + j];
        double curLength = prevA[i * count + k] + prevA[k * count + j];
        if (curLength > prevLength)
          curLength = prevLength;
        curA[i * count + j] = curLength;
        //curA[i * count + j] = prevA[i * count + j] +
        //                      prevA[i * count + k] * prevA[k * count + j];
      }

    //printf("Matrix at max id = %d:" ENDL, k + 1);    
    //sg_PrintFWMatrix(curA, count);
  }
  double ret = INF;
  for (int i = 0; i < count; i++)
  {
    if (curA[i * count + i] < 0)
      throw Exception("A negative loop detected in Floyd-Warshall");
    for (int j = 0; j < count; j++)
      if (curA[i * count + j] < ret)
        ret = curA[i * count + j];
  }
  return ret;
}

double Alg::RunFloydWarshall(const PNodeVector &nodes)
{
  return Alg::RunFloydWarshall(&nodes[0], (int)nodes.size());
}


struct PQItemMST
{
  Node *node;
  LinkData *ld;
  double value;
  bool operator< (const PQItemMST &v) const
  {
    return value > v.value;
  }
};

double Alg::FindUMST(Node * const *nodes, int count, SrcLinkVector &ret)
{
  // node tags: -1: unseen; 0: queued; 1: added to the tree
  ret.clear();
  double totalCost = 0;

  for (int j = 0; j < count; j++)
    nodes[j]->m_tag = -1;

  Node *start = nodes[0];

  std::priority_queue<PQItemMST> heap;

  start->m_tag = 0;
  start->m_dtag = 0;
  start->m_ntag = NULL;
  PQItemMST item = { start, NULL, 0 };
  heap.push(item);

  while (heap.size() > 0)
  {
    PQItemMST top = heap.top();
    heap.pop();
    if (top.node->m_dtag < top.value)
      continue;

    Node *topNode = top.node;
    totalCost += topNode->m_dtag;
    if (topNode->m_ntag != NULL)
    {
      SrcLink srcLink;
      srcLink.src = topNode->m_ntag;
      srcLink.link.d = top.ld;
      srcLink.link.n = topNode;
      ret.push_back(srcLink);
    }
    topNode->m_tag = 1;

    LinkVector &links = topNode->links();
    for (int i = 0; i < (int)links.size(); i++)
    {
      Node *n2 = links[i].n;
      LinkData *ld = links[i].d;

      // node is not in the tree and it's unseen or the current cost is better
      if (n2->m_tag < 1 && (n2->m_tag < 0 || ld->m_weight < n2->m_dtag))
      {
        n2->m_tag = 0;
        n2->m_dtag = ld->m_weight;
        n2->m_ntag = topNode;
        PQItemMST nItem = { n2, ld, ld->m_weight };
        heap.push(nItem);
      }
    }
  }

  if ((int)ret.size() != count - 1)
    throw Exception("MST links number is not equal the number of nodes, "
                    "less 1; the graph may be disconnected");

  return totalCost;
}

double Alg::FindUMST(const PNodeVector &nodes, SrcLinkVector &ret)
{
  return Alg::FindUMST(&nodes[0], (int)nodes.size(), ret);
}


double Alg::FindUMSTKruskal(const PNodeVector &nodes,
                            SrcLinkVector &srcLinks,
                            SrcLinkVector &ret)
{
  const int NC = (int)nodes.size();
  const int LC = (int)srcLinks.size();
  for (int i = 0; i < NC; i++)
    nodes[i]->m_tag = i;

  double totalCost = 0;
  UnionFind ufind(NC);
  for (int i = 0; i < LC; i++)
  {
    SrcLink &srcLink = srcLinks[i];
    Node *n1 = srcLink.src;
    Node *n2 = srcLink.link.n;
    int comp1 = ufind.find(n1->m_tag);
    int comp2 = ufind.find(n2->m_tag);
    if (comp1 == comp2)
      continue;
    else
    {
      totalCost += srcLink.link.d->m_weight;
      ret.push_back(srcLink);
      ufind.mergeDistinctRoots(comp1, comp2);
      if (ret.size() == NC - 1)
        break;
    }
  }
  if ((int)ret.size() != NC - 1)
    throw Exception("MST links number is not equal the number of nodes, "
                    "less 1; the graph may be disconnected");
  return totalCost;
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
  // Node tags: -1: unseen; 0: added to the stack; 1: in-links added; 2: processed
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
      if (sn->m_tag > 1)
        S.pop();
      else if (sn->m_tag == 1)
      {
        S.pop();
        sn->m_tag = 2;
        L.push_front(sn);
      }
      else
      {
        const LinkVector &iLinks = sn->inLinks();
        for (size_t j = 0; j < iLinks.size(); j++)
        {
          Node *src = iLinks[j].n;
          if (src->m_tag <= 0)
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
  // Node tags: 2: unseen; 3: processed
  PNodeQueue Q;
  std::map<Node *, PNodeList *> D;
  for (PNodeList::iterator it = L.begin(); it != L.end(); it++)
  {
    Node *n = *it;
    if (n->m_tag > 2)
      continue;

    PNodeList *l = new PNodeList();
    D[n] = l;

    Q.push(n);
    n->m_tag = 3;
    l->push_back(n);

    while (Q.size() > 0)
    {
      Node *sn = Q.front();
      Q.pop();
      const LinkVector &links = sn->links();
      for (size_t j = 0; j < links.size(); j++)
      {
        Node *dst = links[j].n;
        if (dst->m_tag <= 2)
        {
          Q.push(dst);
          dst->m_tag = 3;
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
