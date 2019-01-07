/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Graphs/graph.h"
#include "Graphs/circnode.h"
#include "Graphs/circnodeparser.h"
#include "Graphs/circnodewriter.h"

#ifndef CIRCGRAPH_HEADER_FILE_INCLUDED
#define CIRCGRAPH_HEADER_FILE_INCLUDED

class LIBGRAPHS_API CircGraph : public Graph
{
protected:
  double generateKappa(double g, double kappaMax);
  void linkNodesCircER(double b, double R);
  void linkNodesCircSF(double b, double R, double g);

  virtual NodeParser<CsvCol> *newNodeCsvParser()
    { return new CircNodeParser<CsvCol>(); }

public:
  CircGraph() : Graph(new NetFactory<CircNode, LinkData>())
  {
  }

  virtual ~CircGraph()
  {
  }

  void GenerateCircER(int n, double k, double b);
  void GenerateCircSF(int n, double k, double g, double b);
};

#endif // CIRCGRAPH_HEADER_FILE_INCLUDED
