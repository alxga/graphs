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
  
  // throws an exception if unequal link lengths are detected
  static double ApproxAvClssBFS(const PNodeVector &nodes, bool activeOnly);
  // if graph/component is not (optionally, actively) strongly connected,
  // an exception may be thrown
  // throws an exception if unequal link lengths are detected
  static double ApproxAvFarnessBFS(const PNodeVector &nodes, bool activeOnly,
                                   int compId = -1);
  // if graph/component is not (optionally, actively) strongly connected,
  // an exception will be thrown
  // throws an exception if unequal link lengths are detected
  // appropriate to use in undirected graphs only
  static double ApproxUDiameterBFS(const PNodeVector &nodes, bool activeOnly,
                                   int compId = -1);

  /*
    Calculates node betweenness, closeness, and farness centralities
    Calculates link betweenness centrality
    Supports a directed or undirected possibly not connected graph
    Requires equal and positive lengths of all active links
    Centralities for inactive nodes and links are set to -1
    Centralities for links incident on inactive nodes are set to -1
    Throws an exception if unequal link lengths are detected
    Betweenness centralities are not normalized
    Closeness centralities are normalized on the count of nodes
    Farness centralities are normalized on the node's connected component size
    \diam is set to the largest distance between any connected pair of nodes
  */
  static void CalcCentralitiesBFS(const PNodeVector &nodes, bool activeOnly,
                                  double *diam = NULL);

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

  // finds a minimum spanning tree of an undirected graph with link costs
  // given by their weights, throws an exception if the graph is not connected
  // supports negative edge costs; srcLinks must be sorted as desired
  static double FindUMSTKruskal(const PNodeVector &nodes,
                                SrcLinkVector &srcLinks,
                                SrcLinkVector &ret);

  static void CalcUAvNNDegree(const PNodeVector &nodes);
  static void CalcUClustering(const PNodeVector &nodes);

  static int AssignUComponentIDs(const PNodeVector &nodes, bool activeOnly);
  static int AssignWkComponentIDs(const PNodeVector &nodes, bool activeOnly);
  static int AssignSgComponentIDs(const PNodeVector &nodes);
};

#endif // ALG_HEADER_FILE_INCLUDED
