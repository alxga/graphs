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
  // uses m_ntag to store an immediate incoming link node on a shortest path,
  // and m_dtag to store the distance from src to the node
  // follows a node's incoming links if forward is false
  // throws an exception if unequal link lengths are detected
  static void CalcDistancesBFS(Node *src, Node * const *nodes, int count,
                               bool activeOnly, bool forward);
  // wrapper function for the previous one
  static void CalcDistancesBFS(Node *src, const PNodeVector &nodes,
                               bool activeOnly, bool forward);

  // updates m_pathTol, -1 implies that a node is not on the path,
  // termini are considered a part of the path
  // throws an exception if unequal link lengths are detected
  static void CalcPathToleranceBFS(Node *src, Node *dst,
                                   const PNodeVector &nodes,
                                   bool activeOnly);

  // if graph/component is not (optionally, actively) strongly connected,
  // an exception may be thrown
  // throws an exception if unequal link lengths are detected
  static double ApproxAvClssBFS(const PNodeVector &nodes, bool activeOnly,
                                int compId = -1);
  // if graph/component is not (optionally, actively) strongly connected,
  // an exception will be thrown
  // throws an exception if unequal link lengths are detected
  // appropriate to use in undirected graphs only
  static double ApproxUDiameterBFS(const PNodeVector &nodes, bool activeOnly,
                                   int compId = -1);
  // if graph/component is not (optionally, actively) strongly connected,
  // an exception will be thrown
  // throws an exception if unequal link lengths are detected
  static void CalcCentralitiesBFS(const PNodeVector &nodes, bool activeOnly,
                                  double *D = NULL, int compId = -1);

  // uses m_ntag to store an immediate incoming link node on the shortest path,
  // and m_dtag to store the distance from src to the node
  // follows a node's incoming links if forward is false
  static void RunDijkstra(Node *src, Node * const *nodes, int count,
                          bool activeOnly, bool forward);
  // wrapper function for the previous one
  static void RunDijkstra(Node *src, const PNodeVector &nodes,
                          bool activeOnly, bool forward);

  // finds a minimum spanning tree of an undirected graph with link costs
  // given by their weights, throws an exception if the graph is not connected
  // supports negative edge costs
  static double FindUMST(Node * const *nodes, int count, SrcLinkVector &ret);
  // wrapper function for the previous one
  static double FindUMST(const PNodeVector &nodes, SrcLinkVector &ret);

  static void CalcUAvNNDegree(const PNodeVector &nodes);
  static void CalcUClustering(const PNodeVector &nodes);

  static int AssignUComponentIDs(const PNodeVector &nodes, bool activeOnly);
  static int AssignWkComponentIDs(const PNodeVector &nodes, bool activeOnly);
  static int AssignSgComponentIDs(const PNodeVector &nodes);
};

#endif // ALG_HEADER_FILE_INCLUDED
