/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/circnodewriter.h"
#include "Graphs/circnode.h"


void CircNodeWriter::writeCsvHeader(std::ostream &os)
{
  NodeWriter::writeCsvHeader(os);
  os << "," << "angle";
  os << "," << "degree";
  os << "," << "annd";
  os << "," << "cl_coef";
  os << "," << "kappa";
  os << "," << "radial";
}

bool CircNodeWriter::writeCsv(std::ostream &os, const Node *node)
{
  CircNode *sn = (CircNode *)node;
  NodeWriter::writeCsv(os, node);
  os << "," << sn->m_angle;
  os << "," << node->numLinks();
  os << "," << node->m_annd;
  os << "," << node->m_clCoef;
  os << "," << sn->m_kappa;
  os << "," << sn->m_radial;
  return true;
}
