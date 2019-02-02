/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Graphs/graph.h"

#ifndef CIRCNODE_HEADER_FILE_INCLUDED
#define CIRCNODE_HEADER_FILE_INCLUDED

class CircNode : public Node
{
public:
  double m_radial;
  double m_angle;
  double m_kappa;

  CircNode() : Node()
  {
    m_radial = -1;
    m_angle = 0;
    m_kappa = 0;
  }

  inline double angDiff(const CircNode &src) const
  {
    return std::abs(signedAngDiff(src));
  }
  double signedAngDiff(const CircNode &src) const
  {
    double d = m_angle - src.m_angle;
    while (d < -M_PI) d += 2 * M_PI;
    while (d > M_PI) d -= 2 * M_PI;
    return d;
  }

  inline double h2Dist(const CircNode &v) const
  {
    // As the coordinates for zero degree nodes are not provided,
    // we return a very high number here so that those nodes are
    // deprioritized in targeted removals
    if (m_radial < 0 || v.m_radial < 0)
      return 1e100;

    double dth = angDiff(v);
    return m_radial + v.m_radial + 2 * std::log(std::sin(dth / 2));
  }
};

typedef std::list<CircNode> CircNodeList;
typedef std::list<CircNode *> PCircNodeList;
typedef std::vector<CircNode> CircNodeVector;
typedef std::vector<CircNode *> PCircNodeVector;

#endif // CIRCNODE_HEADER_FILE_INCLUDED
