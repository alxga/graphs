/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/math.h"
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
    // if angle is too small inverse cosines fail to yield good precision.
    // Instead, I use the exact result 
    if (dth < 1e-7)
      return std::abs(m_radial - v.m_radial);
    
    double ret;
    ret = (std::cosh(m_radial)*std::cosh(v.m_radial)) -
          (std::sinh(m_radial)*std::sinh(v.m_radial) * std::cos(dth));
    ret = Utils::acosh(ret);
    return ret;
  }
};

typedef std::list<CircNode> CircNodeList;
typedef std::list<CircNode *> PCircNodeList;
typedef std::vector<CircNode> CircNodeVector;
typedef std::vector<CircNode *> PCircNodeVector;

#endif // CIRCNODE_HEADER_FILE_INCLUDED
