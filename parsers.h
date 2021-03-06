/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Graphs/libgraphs.h"
#include "Graphs/cols.h"
#include "Graphs/node.h"

#ifndef PARSERS_HEADER_FILE_INCLUDED
#define PARSERS_HEADER_FILE_INCLUDED

template<typename ColType>
  class LIBGRAPHS_API NodeParser
  {
  protected:
    int m_nameIx;
    int m_btwsIx;
    int m_clssIx;
    int m_frnsIx;
    int m_anndIx;
    int m_clCoefIx;
    int m_xIx;
    int m_yIx;

    bool checkGdfColType(GraphFileCols<ColType> &cols,
                          int ix, GdfCol::DataType dt)
    {
      if (ix < 0)
        return true;
      GdfCol &col = (GdfCol &)cols[ix];
      return col.type() == dt;
    }

    virtual void verifyTypes(GraphFileCols<ColType> &cols)
    {
      if (std::is_base_of<GdfCol, ColType>::value)
      {
        if (!checkGdfColType(cols, m_btwsIx, GdfCol::Dbl) ||
            !checkGdfColType(cols, m_clssIx, GdfCol::Dbl) ||
            !checkGdfColType(cols, m_frnsIx, GdfCol::Dbl) ||
            !checkGdfColType(cols, m_anndIx, GdfCol::Dbl) ||
            !checkGdfColType(cols, m_clCoefIx, GdfCol::Dbl) ||
            !checkGdfColType(cols, m_xIx, GdfCol::Dbl) ||
            !checkGdfColType(cols, m_yIx, GdfCol::Dbl))
          throw Exception("Incorrect GDF column type (node)");
      }
    }

    virtual void assignIndices(GraphFileCols<ColType> &cols)
    {
      m_nameIx = cols.find("name");
      if (m_nameIx < 0)
        throw Exception("Unable to find a required node column");
      m_btwsIx = cols.find("btws");
      m_clssIx = cols.find("clss");
      m_frnsIx = cols.find("frns");
      m_anndIx = cols.find("annd");
      m_clCoefIx = cols.find("cl_coef");
      m_xIx = cols.find("lng");
      m_yIx = cols.find("lat");
    }

  public:
    void init(GraphFileCols<ColType> &cols)
    {
      assignIndices(cols);
      verifyTypes(cols);
    }

    virtual void parse(StringVector &strs, Node *node)
    {
      node->m_name = strs[m_nameIx];
      double x = (m_xIx >= 0) ? std::stod(strs[m_xIx]) : 0;
      double y = (m_yIx >= 0) ? std::stod(strs[m_yIx]) : 0;
      node->m_coords.set(x, y);
      if (m_btwsIx >= 0)
        node->m_btws = std::stod(strs[m_btwsIx]);
      if (m_clssIx >= 0)
        node->m_clss = std::stod(strs[m_clssIx]);
      if (m_frnsIx >= 0)
        node->m_frns = std::stod(strs[m_frnsIx]);
      if (m_anndIx >= 0)
        node->m_annd = std::stod(strs[m_anndIx]);
      if (m_clCoefIx >= 0)
        node->m_clCoef = std::stod(strs[m_clCoefIx]);
    }
  };

template<typename ColType>
  class LIBGRAPHS_API LinkParser
  {
  protected:
    int m_nameIx;
    int m_name1Ix;
    int m_name2Ix;
    int m_btwsIx;
    int m_lengthIx;
    int m_weightIx;

    bool checkGdfColType(GraphFileCols<ColType> &cols,
                          int ix, GdfCol::DataType dt)
    {
      if (ix < 0)
        return true;
      GdfCol &col = (GdfCol &)cols[ix];
      return col.type() == dt;
    }

    virtual void verifyTypes(GraphFileCols<ColType> &cols)
    {
      if (std::is_base_of<GdfCol, ColType>::value)
      {
        if (!checkGdfColType(cols, m_btwsIx, GdfCol::Dbl) ||
            !checkGdfColType(cols, m_lengthIx, GdfCol::Dbl) ||
            !checkGdfColType(cols, m_weightIx, GdfCol::Dbl))
          throw Exception("Incorrect GDF column type (link)");
      }
    }

    virtual void assignIndices(GraphFileCols<ColType> &cols)
    {
      m_nameIx = cols.find("name");
      m_name1Ix = cols.find("node1");
      if (m_name1Ix < 0)
        m_name1Ix = cols.find("source");
      m_name2Ix = cols.find("node2");
      if (m_name2Ix < 0)
        m_name2Ix = cols.find("target");
      if (m_name1Ix < 0 || m_name2Ix < 0)
        throw Exception("Unable to find a required link column");
      m_btwsIx = cols.find("btws");
      m_lengthIx = cols.find("length");
      m_weightIx = cols.find("weight");
    }

  public:
    void init(GraphFileCols<ColType> &cols)
    {
      assignIndices(cols);
      verifyTypes(cols);
    }

    virtual void parse(StringVector &strs, LinkData *ld,
                       std::string &name1, std::string &name2)
    {
      if (m_nameIx >= 0)
        ld->m_name = strs[m_nameIx];
      name1 = strs[m_name1Ix];
      name2 = strs[m_name2Ix];

      ld->m_directed = false;

      if (m_btwsIx >= 0)
        ld->m_btws = std::stod(strs[m_btwsIx]);
      if (m_lengthIx >= 0)
        ld->m_length = std::stod(strs[m_lengthIx]);
      if (m_weightIx >= 0)
        ld->m_weight = std::stod(strs[m_weightIx]);
    }
  };

#endif // PARSERS_HEADER_FILE_INCLUDED
