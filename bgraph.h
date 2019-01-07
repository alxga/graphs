/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Graphs/graph.h"
#include "Graphs/bnode.h"

#ifndef BGRAPH_HEADER_FILE_INCLUDED
#define BGRAPH_HEADER_FILE_INCLUDED

class LIBGRAPHS_API BGraph : public Graph
{
protected:
  void linkAndDeleteFromTo(BNode **from, BNode **to, int count, int numLinks);

public:
  BGraph() : Graph(new NetFactory<BNode, LinkData>())
  {
  }

  virtual ~BGraph()
  {
  }

  int DeactDependent(int dactTime);

  void ReadDependencyLinks(const char *path, BGraph &g, bool isMutual);
  void WriteDependencyLinks(const char *path);

  void GenerateCoupling(Graph &g, double q);
};

#endif // BGRAPH_HEADER_FILE_INCLUDED
