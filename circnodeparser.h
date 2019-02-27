/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Graphs/parsers.h"
#include "Graphs/circnode.h"

#ifndef CIRCNODEPARSER_HEADER_FILE_INCLUDED
#define CIRCNODEPARSER_HEADER_FILE_INCLUDED

template<typename ColType>
  class CircNodeParser : public NodeParser<ColType>
  {
  protected:
    int m_radialIx;
    int m_angleIx;
    int m_kappaIx;

    virtual void verifyTypes(GraphFileCols<ColType> &cols)
    {
      NodeParser<ColType>::verifyTypes(cols);

      if (std::is_base_of<GdfCol, ColType>::value)
      {
        if (!this->checkGdfColType(cols, m_angleIx, GdfCol::Dbl) ||
            !this->checkGdfColType(cols, m_radialIx, GdfCol::Dbl) ||
            !this->checkGdfColType(cols, m_kappaIx, GdfCol::Dbl))
          throw Exception("Incorrect node GDF column type");
      }
    }

    virtual void assignIndices(GraphFileCols<ColType> &cols)
    {
      NodeParser<ColType>::assignIndices(cols);

      m_radialIx = cols.find("radial");
      m_angleIx = cols.find("angle");
      m_kappaIx = cols.find("kappa");

      if (m_angleIx < 0 ||
          NodeParser<ColType>::m_xIx < 0 ||
          NodeParser<ColType>::m_yIx < 0)
        throw Exception("Unable to find a required spatial node column");
    }

  public:
    CircNodeParser()
    {
    }

    void init(GraphFileCols<ColType> &cols)
    {
      assignIndices(cols);
      verifyTypes(cols);
    }

    virtual void parse(StringVector &strs, Node *node)
    {
      NodeParser<ColType>::parse(strs, node);

      CircNode *sn = (CircNode *)node;
      sn->m_angle = std::stod(strs[m_angleIx]);
      if (m_kappaIx >= 0)
        sn->m_kappa = std::stod(strs[m_kappaIx]);
      if (m_radialIx >= 0)
        sn->m_radial = std::stod(strs[m_radialIx]);
    }
  };

#endif // CIRCNODEPARSER_HEADER_FILE_INCLUDED
