/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "node.h"

#ifndef ALG_HEADER_FILE_INCLUDED
#define ALG_HEADER_FILE_INCLUDED

class LIBGRAPHS_API Alg
{
public:
  // uses m_ntag to store an immediate incoming link node on the shortest path,
  // and m_dtag to store the distance from src to the node
  // follows a node's incoming links if forward is false
  static void RunDijkstra(Node *src, const PNodeVector &nodes,
                          bool activeOnly, bool forward);
  // uses m_ntag to store an immediate incoming link node on the shortest path,
  // and m_dtag to store the distance from src to the node
  // follows a node's incoming links if forward is false
  static void RunDijkstra(Node *src, Node * const *nodes, int count,
                          bool activeOnly, bool forward);

  // updates m_pathTol, -1 implies that a node is not on the path,
  // termini are considered a part of the path
  // link lengths must be positive or an infinite loop may ensue
  static void CalcPathTolerance(Node *src, Node *dst, const PNodeVector &nodes,
                                bool activeOnly);

  // if graph/component is not (optionally, actively) connected,
  // an exception will be thrown
  static double ApproxUAvClss(const PNodeVector &nodes, bool activeOnly,
                              int compId = -1);
  // if graph/component is not (optionally, actively) connected,
  // an exception will be thrown
  static double ApproxUDiameter(const PNodeVector &nodes, bool activeOnly,
                                int compId = -1);
  // if graph/component is not (optionally, actively) connected,
  // an exception will be thrown
  static void CalcUCentralities(const PNodeVector &nodes, bool activeOnly,
                                double *D = NULL, int compId = -1);

  static void CalcUAvNNDegree(const PNodeVector &nodes);
  static void CalcUClustering(const PNodeVector &nodes);
  
  static int AssignUComponentIDs(const PNodeVector &nodes, bool activeOnly);
  static int AssignWkComponentIDs(const PNodeVector &nodes, bool activeOnly);
  static int AssignSgComponentIDs(const PNodeVector &nodes);
};

class LIBGRAPHS_API AlgDeprecated
{
public:
  // the 2 functions below are deprecated and suboptimal

  // uses m_ntag to store an immediate incoming link node on a shortest path,
  // and m_dtag to store the distance from src to the node
  // follows a node's incoming links if forward is false
  static void CalcDistances(Node *src, const PNodeVector &nodes,
                             bool activeOnly, bool forward);
  // uses m_ntag to store an immediate incoming link node on the shortest path,
  // and m_dtag to store the distance from src to the node
  // follows a node's incoming links if forward is false
  static void CalcDistances(Node *src, Node * const *nodes, int count,
                             bool activeOnly, bool forward);
};

#endif // ALG_HEADER_FILE_INCLUDED
