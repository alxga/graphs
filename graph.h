/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/utils.h"
#include "Graphs/libgraphs.h"
#include "Graphs/netfactory.h"
#include "Graphs/blkmem.h"
#include "Graphs/sfdistr.h"
#include "parsers.h"
#include "writers.h"


#ifndef GRAPH_HEADER_FILE_INCLUDED
#define GRAPH_HEADER_FILE_INCLUDED


class LIBGRAPHS_API Graph
{
protected:
  INetFactory *m_netFactory;
  PNodeVector *m_nodes;
  StrPNodeMap *m_nodeMap;
  PLinkDataVector *m_linkData;
  
  void resizeAndResetNodes(size_t n);

  void linkByDistance(double r, Node *nd, PNodeList &lst, bool sameBin);
  
  bool linkNodesByTags(int sumDegrees);
  bool linkNodesByDistribution(IDoubleDistr *distr);
  bool linkNodesBA(int k);
  void linkNodesRLat(int a, int b);
  void linkNodesSpatial(double r, double w, double h);

  virtual Node *addNodeSimple(const std::string &name);
  virtual void linkSimple(Node *n1, Node *n2);

  Node **m_nodesByDegree;
  Node **nodesByDegree();

  virtual NodeParser<CsvCol> *newNodeCsvParser()
    { return new NodeParser<CsvCol>(); }
  virtual LinkParser<CsvCol> *newLinkCsvParser()
    { return new LinkParser<CsvCol>(); }
  virtual NodeWriter *newNodeWriter() { return new NodeWriter(); }
  virtual LinkWriter *newLinkWriter() { return new LinkWriter(); }

public:
  Graph(INetFactory *netFactory = NULL)
  {
    if (netFactory == NULL)
      netFactory = new NetFactory<Node, LinkData>();
    m_netFactory = netFactory;
    m_nodesByDegree = NULL;

    m_nodes = new PNodeVector();
    m_nodeMap = new StrPNodeMap();
    m_linkData = new PLinkDataVector();
  }

  virtual ~Graph()
  {
    delete m_linkData;
    delete m_nodeMap;
    delete m_nodes;

    delete m_netFactory;
    delete [] m_nodesByDegree;
  }

  Node *parseNode(StringVector &strs, NodeParser<CsvCol> *parser);
  LinkData *parseLinkData(StringVector &strs, LinkParser<CsvCol> *parser);

  inline void updateNumActLinks()
  {
    for (size_t i = 0; i < m_nodes->size(); i++)
      (*m_nodes)[i]->updateNumActLinks();
  }

  inline int nCount() const { return (int)m_nodes->size(); }
  inline int ldCount() const { return (int)m_linkData->size(); }

  inline PNodeVector &nodes() const { return *m_nodes; }
  inline PLinkDataVector &linkData() const { return *m_linkData; }
  inline StrPNodeMap &nodeMap() { return *m_nodeMap; }

  double calcAvNumLinks() const;

  void getSrcLinks(SrcLinkVector &ret) const;
  void getActSrcLinks(SrcLinkVector &ret) const;

  size_t ActAll();
  size_t Deact(size_t needDeact, size_t haveAct, int dactTime);

  size_t ActAllLinks();
  size_t DeactLinks(size_t needDeact, size_t haveAct, int dactTime);

  int DeactSubGraphComponents(int dactTime);
  
  void GenerateER(int n, double k);
  void GenerateRR(int n, int k);
  void GenerateBA(int n, int m);
  void GenerateSF(int n, double k, double g, int min);
  void GenerateMKSF(int n, double k, double g);
  void GenerateSFStrictly(int n, double k, double g);
  void GenerateRLat(int n);
  void GenerateSpatial(int n, double r, double w = 1, double h = 1);

  void URewire();

  virtual void ReadCsv(const char *nPath, const char *lPath);
  virtual void WriteNodesCsv(const char *path, NodeWriter *writer = 0);
  virtual void WriteLinksCsv(const char *path, LinkWriter *writer = 0);

  virtual void WriteGdf(const char *gPath,
                        NodeWriter *nwriter = 0, LinkWriter *lwriter = 0);

  void ReadAdjacency(const char *path, bool prune = false, int n = -1);
  void WriteAdjacency(const char *fileName, bool gccOnly);

  double NodesForFraction(double f);
  double LinksForFraction(double f);
};

#endif // GRAPH_HEADER_FILE_INCLUDED
