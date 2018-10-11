/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/graph.h"
#include "Graphs/alg.h"


static bool check_tag(Node *n) { return n->m_tag < 2; }

bool Graph::linkNodesByTags(int sumDegrees, bool strict)
{
  PNodeVector &nv = nodes();

  Node **nodes = new Node *[sumDegrees];
  auto_del<Node *> del_nodes(nodes, true);

  int ix = 0;
  for (size_t i = 0; i < nv.size(); i++)
  {
    const int d = nv[i]->m_tag;
    for (int j = 0; j < d; j++)
      nodes[ix++] = nv[i];
  }

  while (sumDegrees > 1)
  {
    int r1, r2;
    Node *n1, *n2;

    r1 = (int) (sumDegrees * RAND_0_1);
    n1 = nodes[r1];
    
    if (!strict)
    {
      r2 = (int) (sumDegrees * RAND_0_1);
      n2 = nodes[r2];
      
      // remove nodes from the list
      nodes[r1] = nodes[sumDegrees - 1];
      nodes[r2] = nodes[sumDegrees - 2];
      sumDegrees -= 2;

      // link nodes if it is acceptable
      if (n1 != n2 && n1->findLink(n2) == NULL)
        linkSimple(n1, n2);
    }
    else
    {
      int attempts = 0;
      do // try to find an acceptable node to link to
      {
        r2 = (int) (sumDegrees * RAND_0_1);
        n2 = nodes[r2];
        attempts ++;
        if (attempts > 100)
        {
          n2 = NULL;
          for (r2 = 0; r2 < sumDegrees; r2++)
          {
            Node *t = nodes[r2];
            if (t != n1 && n1->findLink(t) == NULL)
            {
              n2 = t;
              break;
            }
          }
          break;
        }
      }
      while (n2 == n1 || n1->findLink(n2) != NULL);

      if (n2 == NULL)
      {
        // no acceptable nodes to link to, remove node 1 from the list
        ix = 0;
        Node **tmp = new Node *[sumDegrees];
        for (int i = 0; i < sumDegrees; i++)
          if (nodes[i] != n1)
            tmp[ix++] = nodes[i];
        delete [] nodes;
        nodes = tmp;
        del_nodes.setPtr(tmp);
        sumDegrees = ix;
      }
      else
      {
        // link nodes and remove them from the list
        linkSimple(n1, n2);
        nodes[r1] = nodes[sumDegrees - 1];
        nodes[r2] = nodes[sumDegrees - 2];
        sumDegrees -= 2;
      }
    }
  }

  return true;
}

bool Graph::linkNodesByDistribution(IDoubleDistr *distr)
{
  int maxKIndex = 0;
  int sum = 0;
  double dSum = 0;
  const int nc = nCount();
  PNodeVector &nv = nodes();
  for (int i = 0; i < nc; i++)
  {
    double dtmp = distr->genDouble();
    int tmp = (int)ceil(dtmp);
    nv[i]->m_tag = tmp;
    sum += tmp;
    dSum += dtmp;
    if (nv[maxKIndex]->m_tag < nv[i]->m_tag)
      maxKIndex = i;
  }
  if (sum % 2)
  {
    sum++;
    nv[0]->m_tag++;
  }
  return linkNodesByTags(sum, true);
}

class BACdfIndexer : public ICdfIndexer<Node>
{
  int m_count;
  double *m_cdfs;

  int divHalf(int lb, int ub, double val)
  {
    if (ub - lb < 10)
    {
      for (int i = lb; i <= ub; i++)
        if (m_cdfs[i] > val)
          return i;
    }
    else
    {
      int c = lb + (ub - lb) / 2;
      if (m_cdfs[c] > val)
        return divHalf(lb, c, val);
      else
        return divHalf(c, ub, val);
    }
    return -1;
  }

  void calcCdfs(Node **nodes)
  {
    int totalK = 0;
    for (int i = 0; i < m_count; i++)
    {
      const Node *n = nodes[i];
      totalK += n->numLinks();
      m_cdfs[i] = totalK;
    }
    for (int i = 0; i < m_count; i++)
      m_cdfs[i] /= totalK;
  }
public:
  BACdfIndexer(int count, Node **nodes)
  {
    m_count = count;
    m_cdfs = new double [m_count];
    calcCdfs(nodes);
  }
  virtual ~BACdfIndexer()
  {
    delete [] m_cdfs;
  }
  virtual int index(double cdf)
  {
    return divHalf(0, m_count - 1, cdf);
  }
  virtual void reinit(int count, Node **ptr)
  {
    m_count = count;
    calcCdfs(ptr);
  }
};

bool Graph::linkNodesBA(int m)
{
  PNodeVector &nv = nodes();
  Node **pNodes = new Node *[nv.size()];
  memcpy(pNodes, &m_nodes[0], nv.size() * sizeof(Node *));
  RandomSelector<Node> rsel(pNodes, (int)nv.size(), NULL);

  int connCount = 0;
  Node **connected = new Node *[nv.size()];
  auto_del<Node *> del_conn(connected, true);
  connected[connCount++] = rsel.sel();

  while (rsel.remains() > 0)
  {
    Node *c = rsel.sel();
    if (connCount <= m)
      for (int i = 0; i < connCount; i++)
        linkSimple(c, connected[i]);
    else
    {
      int k = m;
      Node **ptr = new Node *[connCount];
      memcpy(ptr, connected, connCount * sizeof(Node *));
      BACdfIndexer *indexer = new BACdfIndexer(connCount, ptr);
      RandomSelector<Node> baSel(ptr, connCount, indexer);
      while (k > 0)
      {
        Node *n = baSel.sel();
        linkSimple(c, n);
        k --;
      }
    }
    connected[connCount++] = c;
  }

  return true;
}

void Graph::linkNodesRLat(int a, int b)
{
  PNodeVector &nv = nodes();

  for (int i = 0; i < a; i++)
  {
    int i2 = (i + 1) % a;
    for (int j = 0; j < b; j++)
    {
      nv[i * b + j]->m_coords.set(i, j);
      linkSimple(nv[i * b + j], nv[i2 * b + j]);
    }
  }
  for (int i = 0; i < a; i++)
    for (int j = 0; j < b; j++)
      linkSimple(nv[i * b + j], nv[i * b + (j + 1) % b]);
}

void Graph::linkByDistance(double r, Node *nd, PNodeList &bin, bool sameBin)
{
  for (PNodeList::iterator it = bin.begin(); it != bin.end(); it++)
  {
    Node *oth = *it;
    if (!sameBin || nd->m_tag < oth->m_tag)
      if (nd->m_coords.distance2D((*it)->m_coords) <= r)
        linkSimple(nd, *it);
  }
}

void Graph::linkNodesSpatial(double r, double w, double h)
{
  const PNodeVector &ns = nodes();
  const int nc = nCount();
  for (int i = 0; i < nc; i++)
  {
    Node *nd = ns[i];
    nd->m_coords.set(RAND_0_1 * w, RAND_0_1 * h);
    nd->m_tag = i;
  }

  if (r <= 0)
    return;

  int szx = (int) ceil(w / r);
  int szy = (int) ceil(h / r);
  PNodeList **bins = new PNodeList *[szx];
  for (int i = 0; i < szx; i++)
    bins[i] = new PNodeList[szy];

  for (int i = 0; i < nc; i++)
  {
    int ix = (int) (ns[i]->m_coords.x() / r);
    int iy = (int) (ns[i]->m_coords.y() / r);
    bins[ix][iy].push_back(ns[i]);
  }

  for (int i = 0; i < nc; i++)
  {
    Node *nd = ns[i];
    int ix = (int) (nd->m_coords.x() / r);
    int iy = (int) (nd->m_coords.y() / r);
    linkByDistance(r, nd, bins[ix][iy], true);
    if (ix + 1 < szx)
      linkByDistance(r, nd, bins[ix + 1][iy], false);
    if (iy + 1 < szy)
      linkByDistance(r, nd, bins[ix][iy + 1], false);
    if (ix + 1 < szx && iy + 1 < szy)
      linkByDistance(r, nd, bins[ix + 1][iy + 1], false);
  }

  for (int i = 0; i < szx; i++)
    delete [] bins[i];
  delete [] bins;
}




class IFunc
{
public:
  virtual double calc(double g) = 0;
};

class NodesFunc : public IFunc
{
  Graph &m_g;
  double m_target;
public:
  NodesFunc(Graph &g, double target) : m_g(g), m_target(target)
  {
  }

  virtual double calc(double c)
  {
    const size_t C = m_g.nodes().size();
    const PNodeVector &nodes = m_g.nodes();
    m_g.ActAll();
    m_g.Deact((int) (C * (1 - c)), C, 0);
    Alg::AssignUComponentIDs(nodes, true);
    int sum = 0;
    for (size_t i = 0; i < C; i++)
      if (nodes[i]->m_compId == 0)
        sum ++;
    return (double)sum / C - m_target;
  }
};

class LinksFunc : public IFunc
{
  Graph &m_g;
  double m_target;
public:
  LinksFunc(Graph &g, double target) : m_g(g), m_target(target)
  {
  }

  virtual double calc(double c)
  {
    const size_t C = m_g.nodes().size();
    const size_t LC = m_g.linkData().size();
    const PNodeVector &nodes = m_g.nodes();
    m_g.ActAllLinks();
    m_g.DeactLinks((int) (LC * (1 - c)), LC, 0);    
    Alg::AssignUComponentIDs(nodes, true);
    int sum = 0;
    for (size_t i = 0; i < C; i++)
      if (nodes[i]->m_compId == 0)
        sum ++;
    return (double)sum / C - m_target;
  }
};

static double divHalf(double a, double b, IFunc *func)
{
  const double eps = 1E-3;

  double prevC = a - 1;
  double c = (a + b) / 2;
  double f = func->calc(c);

  while (std::abs(f) > eps && std::abs(c - prevC) > 1E-10)
  {
    if (f < 0)
      a = c;
    else
      b = c;

    prevC = c;
    c = (a + b) / 2;
    f = func->calc(c);
  }

  return c;
}


void Graph::getSrcLinks(SrcLinkVector &ret) const
{
  PNodeVector &nv = nodes();

  ret.clear();
  for (size_t i = 0; i < nv.size(); i++)
  {
    LinkVector &nLinks = nv[i]->links();
    for (size_t j = 0; j < nLinks.size(); j++)
    {
      SrcLink sl = { nv[i], nLinks[j] };
      ret.push_back(sl);
    }
  }
}

void Graph::getActSrcLinks(SrcLinkVector &ret) const
{
  PNodeVector &nv = nodes();

  ret.clear();
  for (size_t i = 0; i < nv.size(); i++)
  {
    LinkVector &nLinks = nv[i]->links();
    for (size_t j = 0; j < nLinks.size(); j++)
      if (nLinks[j].d->m_dactTime < 0)
      {
        SrcLink sl = { nv[i], nLinks[j] };
        ret.push_back(sl);
      }
  }
}

size_t Graph::ActAll()
{
  PNodeVector &nv = nodes();

  for (size_t i = 0; i < nv.size(); i++)
    nv[i]->m_dactTime = -1;
  return nv.size();
}

size_t Graph::Deact(size_t needDeact, size_t haveAct, int dactTime)
{
  PNodeVector &nv = nodes();

  size_t ret = 0;
  double p1 = haveAct > 0 ? (double)needDeact / haveAct : 0;
  for (size_t i = 0; i < nv.size(); i++)
  {
    if (nv[i]->m_dactTime >= 0)
      continue;
    if (p1 < RAND_0_1)
      ret++;
    else
    {
      nv[i]->m_dactTime = dactTime;
      needDeact--;
    }
    haveAct --;
    p1 = haveAct > 0 ? (double)needDeact / haveAct : 0;
  }
  return ret;
}

size_t Graph::ActAllLinks()
{
  PLinkDataVector &lData = linkData();

  for (size_t i = 0; i < lData.size(); i++)
    lData[i]->m_dactTime = -1;
  return lData.size();
}

size_t Graph::DeactLinks(size_t needDeact, size_t haveAct, int dactTime)
{
  PLinkDataVector &lData = linkData();

  size_t ret = 0;
  double p1 = haveAct > 0 ? (double)needDeact / haveAct : 0;
  for (size_t i = 0; i < lData.size(); i++)
  {
    if (lData[i]->m_dactTime >= 0)
      continue;
    if (p1 < RAND_0_1)
      ret++;
    else
    {
      lData[i]->m_dactTime = dactTime;
      needDeact--;
    }
    haveAct --;
    p1 = haveAct > 0 ? (double)needDeact / haveAct : 0;
  }
  return ret;
}

int Graph::DeactSubGraphComponents(int dactTime)
{
  PNodeVector &nv = nodes();

  Alg::AssignUComponentIDs(nv, true);

  int ret = 0;
  for (size_t i = 0; i < nv.size(); i++)
  {
    if (nv[i]->m_compId == 0)
    {
      nv[i]->m_dactTime = -1;
      ret++;
    }
    else
      nv[i]->m_dactTime = dactTime;
  }
  return ret;
}

static int sg_compNodesByDegree(const void *v1, const void *v2)
{
  const Node *n1 = *((const Node **) v1);
  const Node *n2 = *((const Node **) v2);
  return n2->numLinks() - n1->numLinks();
}

Node **Graph::nodesByDegree()
{
  if (m_nodesByDegree == NULL)
  {
    PNodeVector &nv = nodes();
    m_nodesByDegree = new Node *[nv.size()];
    memcpy(m_nodesByDegree, &m_nodes[0], nv.size() * sizeof(Node *));
    std::qsort(m_nodesByDegree, nv.size(),
               sizeof(Node *), sg_compNodesByDegree);
  }
  return m_nodesByDegree;
}

void Graph::resizeAndResetNodes(size_t n)
{
  m_nodes->clear();
  m_nodeMap->clear();
  m_linkData->clear();
  m_netFactory->clearNodes();
  m_netFactory->clearLinks();
  for (size_t i = 0; i < n; i++)
    addNodeSimple(ToString(i));

  delete [] m_nodesByDegree;
  m_nodesByDegree = NULL;
}


Node *Graph::parseNode(StringVector &strs, NodeParser<CsvCol> *parser)
{
  Node *n = m_netFactory->nextNode();
  parser->parse(strs, n);
  m_nodes->push_back(n);
  (*m_nodeMap)[n->m_name] = n;
  return n;
}

LinkData *Graph::parseLinkData(StringVector &strs, LinkParser<CsvCol> *parser)
{
  LinkData *ld = m_netFactory->nextLink();
  std::string name1, name2;
  parser->parse(strs, ld, name1, name2);
  Node *n1 = (*m_nodeMap)[name1];
  Node *n2 = (*m_nodeMap)[name2];
  m_linkData->push_back(ld);
  n1->link(n2, ld);
  if (!ld->m_directed)
    n2->link(n1, ld);
  return ld;
}


Node *Graph::addNodeSimple(const std::string &name)
{
  Node *n = m_netFactory->nextNode();
  n->m_name = name;
  m_nodes->push_back(n);
  (*m_nodeMap)[n->m_name] = n;
  return n;
}

void Graph::linkSimple(Node *n1, Node *n2)
{
  LinkData *ld = m_netFactory->nextLink();
  ld->m_directed = false;
  (*m_linkData).push_back(ld);
  n1->link(n2, ld);
  n2->link(n1, ld);
}


void Graph::GenerateER(int n, double k)
{
  resizeAndResetNodes(n);

  double lp = k / nCount();

  BoostBinDistr dist(n, lp);
  linkNodesByDistribution(&dist);
}

void Graph::GenerateRR(int n, int k)
{
  resizeAndResetNodes(n);

  RegularDistr dist(k);
  while (!linkNodesByDistribution(&dist))
    ; // INTENDED
}

void Graph::GenerateBA(int n, int m)
{
  resizeAndResetNodes(n);

  while (!linkNodesBA(m))
    ; // INTENDED
}

void Graph::GenerateSF(int n, double k, double g, int min)
{
  resizeAndResetNodes(n);

  SFDegreeDistr dist;
  dist.initialize(nCount(), k, g, min);
  while (!linkNodesByDistribution(&dist))
    ; // INTENDED
}

void Graph::GenerateMKSF(int n, double k, double g)
{
  resizeAndResetNodes(n);

  MKSFDegreeDistr dist;
  dist.initialize(nCount(), k, g);
  while (!linkNodesByDistribution(&dist))
    ; // INTENDED
}

void Graph::GenerateSFStrictly(int n, double k, double g)
{
  resizeAndResetNodes(n);

  StrictSFDegreeDistr dist;
  dist.initialize(nCount(), k, g);
  while (!linkNodesByDistribution(&dist))
    dist.reset(); // INTENDED
}

void Graph::GenerateRLat(int n)
{
  int a = (int)std::sqrt((double) n);
  int b = a;
  while (a * b != n && a > 10)
  {
    a--;
    b = n / a;
  }
  if (a <= 10)
    throw Exception("Unable to generate a rectangular lattice");

  resizeAndResetNodes(n);
  linkNodesRLat(a, b);
}

void Graph::GenerateSpatial(int n, double r, double w, double h)
{
  resizeAndResetNodes(n);
  linkNodesSpatial(r, w, h);
}

void Graph::URewire(bool strict, int *degrees)
{
  PNodeVector &nv = nodes();
  const int nc = nCount();
  int sum = 0;
  for (int i = 0; i < nc; i++)
  {
    Node *n = nv[i];
    n->m_tag = degrees ? degrees[i] : n->numLinks();
    n->reinitLinks();
    sum += n->m_tag;
  }
  m_linkData->clear();
  m_netFactory->clearLinks();
  
  linkNodesByTags(sum, strict);
}

void Graph::ClearNodesByPredicate(bool (*chk)(Node *), bool rename,
                                  PNodeVector *removed)
{
  SrcLinkVector srcLinks;
  getSrcLinks(srcLinks);

  m_linkData->clear();
  delete m_nodeMap;
  m_nodeMap = new StrPNodeMap();
  
  const int nc = nCount();
  PNodeVector *oNodes = m_nodes;
  auto_del<PNodeVector> del_oNodes(oNodes, false);
  for (int i = 0; i < nc; i++)
  {
    Node *n = (*oNodes)[i];
    n->m_tag = chk(n);
  }
  m_nodes = new PNodeVector();

  for (int i = 0; i < nc; i++)
  {
    Node *n = (*oNodes)[i];
    if (n->m_tag > 0)
    {
      n->reinitLinks();
      if (rename)
        n->m_name = ToString(m_nodes->size());
      m_nodes->push_back(n);
      (*m_nodeMap)[n->m_name] = n;
    }
    else if (removed != NULL)
      removed->push_back(n);
  }
  for (size_t i = 0; i < srcLinks.size(); i++)
  {
    Node *src = srcLinks[i].src;
    Node *dst = srcLinks[i].link.n;
    LinkData *ld = srcLinks[i].link.d;
    if (src->m_tag && dst->m_tag)
    {
      if (ld->m_directed || src < dst)
        m_linkData->push_back(ld);
      src->link(dst, ld);
    }
  }
}


static int sg_sfDegreeCutoff = 0;
static bool sg_isSfDegreeBelowCutoff(Node *nd)
{
  return nd->numLinks() < sg_sfDegreeCutoff;
}

void Graph::ImposeDegreeCutoff(int cutoff)
{
  PNodeVector removed;
  sg_sfDegreeCutoff = cutoff;
  ClearNodesByPredicate(sg_isSfDegreeBelowCutoff, true, &removed);
  for (size_t i = 0; i < removed.size(); i++)
  {
    Node *n = removed[i];
    n->reinitLinks();
    n->m_name = ToString(m_nodes->size());
    m_nodes->push_back(n);
    (*m_nodeMap)[n->m_name] = n;
  }
}

void Graph::ReadCsv(const char *nPath, const char *lPath)
{
  std::string str;
  CsvCols nCols, lCols;

  NodeParser<CsvCol> *nparser = newNodeCsvParser();
  auto_del<NodeParser<CsvCol>> del_nparser(nparser, false);

  std::ifstream nifs(nPath, std::ios_base::in | std::ios_base::binary);

  if (!nifs.good())
    throw Exception("Unable to open the nodes input file");
  
  std::getline(nifs, str);
  nCols.parse(str);
  nparser->init(nCols);
  while (std::getline(nifs, str))
  {
    Trim(str);
    if (str.empty()) continue; // skip empty lines

    StringVector strs;
    SplitStr(str, strs, ",");
    parseNode(strs, nparser);
  }
  
  LinkParser<CsvCol> *lparser = newLinkCsvParser();
  auto_del<LinkParser<CsvCol>> del_lparser(lparser, false);

  std::ifstream lifs(lPath, std::ios_base::in | std::ios_base::binary);

  if (!lifs.good())
    throw Exception("Unable to open the links input file");
  
  std::getline(lifs, str);
  lCols.parse(str);
  lparser->init(lCols);
  while (std::getline(lifs, str))
  {
    Trim(str);
    if (str.empty()) continue; // skip empty lines

    StringVector strs;
    SplitStr(str, strs, ",");
    parseLinkData(strs, lparser);
  }
}

void Graph::WriteNodesCsv(const char *nPath, NodeWriter *writer)
{
  NodeWriter *nwriter = writer ? writer : newNodeWriter();
  auto_del<NodeWriter> del_nwriter(nwriter, false);

  std::ofstream nifs(nPath, std::ios_base::out | std::ios_base::binary);
  if (!nifs.good())
    throw Exception("Unable to open the nodes output file");

  nwriter->writeCsvHeader(nifs);
  nifs << CSVENDL;

  PNodeVector &nv = nodes();
  for (size_t i = 0; i < nv.size(); i++)
  {
    if (nwriter->writeCsv(nifs, nv[i]))
      nifs << CSVENDL;
  }
}

void Graph::WriteLinksCsv(const char *lPath, LinkWriter *writer)
{
  LinkWriter *lwriter = writer ? writer : newLinkWriter();
  auto_del<LinkWriter> del_lwriter(lwriter, false);

  std::ofstream lifs(lPath, std::ios_base::out | std::ios_base::binary);
  if (!lifs.good())
    throw Exception("Unable to open the links output file");

  lwriter->writeCsvHeader(lifs);
  lifs << CSVENDL;
  
  SrcLinkVector srcLinks;
  getSrcLinks(srcLinks);
  for (size_t i = 0; i < srcLinks.size(); i++)
  {
    SrcLink &sl = srcLinks[i];
    if (sl.link.d->m_directed ||
        sl.src->m_name < sl.link.n->m_name)
    {
      if (lwriter->writeCsv(lifs, sl.link.d, sl.src, sl.link.n))
        lifs << CSVENDL;
    }
  }
}


void Graph::WriteGdf(const char *gPath,
                     NodeWriter *nwr, LinkWriter *lwr)
{
  NodeWriter *nwriter = nwr ? nwr : newNodeWriter();
  auto_del<NodeWriter> del_nwriter(nwriter, false);

  std::ofstream gfs(gPath, std::ios_base::out | std::ios_base::binary);
  if (!gfs.good())
    throw Exception("Unable to open the graph output file");

  nwriter->writeGdfHeader(gfs);
  gfs << CSVENDL;

  PNodeVector &nv = nodes();
  for (size_t i = 0; i < nv.size(); i++)
  {
    if (nwriter->writeGdf(gfs, nv[i]))
      gfs << CSVENDL;
  }

  LinkWriter *lwriter = lwr ? lwr : newLinkWriter();
  auto_del<LinkWriter> del_lwriter(lwriter, false);

  lwriter->writeGdfHeader(gfs);
  gfs << CSVENDL;
  
  SrcLinkVector srcLinks;
  getSrcLinks(srcLinks);
  for (size_t i = 0; i < srcLinks.size(); i++)
  {
    SrcLink &sl = srcLinks[i];
    if (sl.link.d->m_directed ||
        sl.src->m_name < sl.link.n->m_name)
    {
      if (lwriter->writeGdf(gfs, sl.link.d, sl.src, sl.link.n))
        gfs << CSVENDL;
    }
  }
}


void Graph::ReadAdjacency(const char *path, bool prune, int n)
{
  std::ifstream ifs(path, std::ios_base::in | std::ios_base::binary);
  if (!ifs.good())
    throw Exception("Unable to open the adjacency input file");

  std::string line;
  while (std::getline(ifs, line))
  {
    Trim(line);

    StringVector strs;
    SplitStr(line, strs, " ");
    std::string &name1 = strs[0];
    std::string &name2 = strs[1];

    Node *n1, *n2;
    if (m_nodeMap->find(name1) == m_nodeMap->end())
      n1 = addNodeSimple(name1);
    else
      n1 = (*m_nodeMap)[name1];
    if (m_nodeMap->find(name2) == m_nodeMap->end())
      n2 = addNodeSimple(name2);
    else
      n2 = (*m_nodeMap)[name2];

    linkSimple(n1, n2);
  }

  if (prune)
  {
    for (int i = 0; i < nCount(); i++)
      (*m_nodes)[i]->m_name = ToString(i);
    for (int i = nCount(); i < n; i++)
      addNodeSimple(ToString(i));
    m_nodeMap->clear();
    for (int i = 0; i < nCount(); i++)
      (*m_nodeMap)[(*m_nodes)[i]->m_name] = (*m_nodes)[i];
  }
}

void Graph::WriteAdjacency(const char *fileName, bool gccOnly)
{
  const int nc = nCount();
  PNodeVector &nv = nodes();
  if (gccOnly)
    Alg::AssignUComponentIDs(nv, false);

  int offset = 0;
  for (int i = 0; i < nc; i++)
  {
    Node &n = *nv[i];
    if (n.numLinks() > 0 && (!gccOnly || n.m_compId == 0))
      n.m_tag = i - offset;
    else
      offset ++;
  }

  FILE *f = fopen(fileName, "wb");
  auto_file a_f(f);
  for (size_t i = 0; i < nv.size(); i++)
  {
    Node &n1 = *nv[i];
    if (!gccOnly || n1.m_compId == 0)
    {
      const LinkVector &n1Links = n1.links();
      PNodeVector saved; // prune duplicate links
      for (size_t j = 0; j < n1Links.size(); j++)
      {
        Node *n2 = n1Links[j].n;
        if (n1.m_tag < n2->m_tag)
        {
          bool alreadyWritten = false;
          for (size_t k = 0; k < saved.size(); k++)
            if (saved[k]->m_tag == n2->m_tag)
            {
              alreadyWritten = true;
              break;
            }
          if (!alreadyWritten)
          {
            fprintf(f, "%d %d" CSVENDL, n1.m_tag, n2->m_tag);
            saved.push_back(n2);
          }
        }
      }
    }
  }
}


double Graph::NodesForFraction(double f)
{
  NodesFunc nf(*this, f);
  return 1 - divHalf(0, 1, &nf);
}

double Graph::LinksForFraction(double f)
{
  LinksFunc lf(*this, f);
  return 1 - divHalf(0, 1, &lf);
}
