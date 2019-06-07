/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/writers.h"


void NodeWriter::writeCsvHeader(std::ostream &os)
{
  os << "name";
  os << "," << "lat";
  os << "," << "lng";
  if (m_writeCentralities)
  {
    os << "," << "btws";
    os << "," << "clss";
    os << "," << "frns";
  }
}

bool NodeWriter::writeCsv(std::ostream &os, const Node *node)
{
  const int prec = 10;

  os << node->m_name;
  os << "," << std::setprecision(prec) << node->lat();
  os << "," << std::setprecision(prec) << node->lng();
  if (m_writeCentralities)
  {
    os << "," << std::setprecision(prec) << node->m_btws;
    os << "," << std::setprecision(prec) << node->m_clss;
    os << "," << std::setprecision(prec) << node->m_frns;
  }

  return true;
}

void NodeWriter::writeGdfHeader(std::ostream &os)
{
  os << "nodedef> ";

  os << "name VARCHAR";
  os << "," << "lat DOUBLE";
  os << "," << "lng DOUBLE";
  if (m_writeCentralities)
  {
    os << "," << "btws DOUBLE";
    os << "," << "clss DOUBLE";
    os << "," << "frns DOUBLE";
  }
}

bool NodeWriter::writeGdf(std::ostream &os, const Node *node)
{
  const int prec = 10;

  os << node->m_name;
  os << "," << std::setprecision(prec) << node->lat();
  os << "," << std::setprecision(prec) << node->lng();
  if (m_writeCentralities)
  {
    os << "," << std::setprecision(prec) << node->m_btws;
    os << "," << std::setprecision(prec) << node->m_clss;
    os << "," << std::setprecision(prec) << node->m_frns;
  }

  return true;
}


void LinkWriter::writeCsvHeader(std::ostream &os)
{
  os << "name";
  os << "," << "node1";
  os << "," << "node2";
  os << "," << "directed";
  os << "," << "length";
  if (m_writeCentralities)
  {
    os << "," << "btws";
    os << "," << "clss";
    os << "," << "frns";
  }
  if (m_writeWeights)
    os << "," << "weight";
}

bool LinkWriter::writeCsv(std::ostream &os, const LinkData *ld,
                          const Node *n1, const Node *n2)
{
  const int prec = 10;

  os << ld->m_name;
  os << "," << n1->m_name;
  os << "," << n2->m_name;
  os << "," << (ld->m_directed ? "true" : "false");
  os << "," << std::setprecision(prec) << ld->m_length;
  if (m_writeCentralities)
  {
    os << "," << std::setprecision(prec) << ld->m_btws;
    os << "," << std::setprecision(prec) << (n1->m_clss + n2->m_clss) / 2;
    os << "," << std::setprecision(prec) << (n1->m_frns + n2->m_frns) / 2;
  }
  if (m_writeWeights)
    os << "," << std::setprecision(prec) << ld->m_weight;

  return true;
}

void LinkWriter::writeGdfHeader(std::ostream &os)
{
  os << "edgedef> ";

  os << "name VARCHAR";
  os << "," << "node1 VARCHAR";
  os << "," << "node2 VARCHAR";
  os << "," << "directed BOOLEAN";
  os << "," << "length DOUBLE";
  if (m_writeCentralities)
  {
    os << "," << "btws DOUBLE";
    os << "," << "clss DOUBLE";
    os << "," << "frns DOUBLE";
  }
  if (m_writeWeights)
    os << "," << "weight DOUBLE";
}

bool LinkWriter::writeGdf(std::ostream &os, const LinkData *ld,
                          const Node *n1, const Node *n2)
{
  const int prec = 10;

  os << ld->m_name;
  os << "," << n1->m_name;
  os << "," << n2->m_name;
  os << "," << (ld->m_directed ? "true" : "false");
  os << "," << std::setprecision(prec) << ld->m_length;
  if (m_writeCentralities)
  {
    os << "," << std::setprecision(prec) << ld->m_btws;
    os << "," << std::setprecision(prec) << (n1->m_clss + n2->m_clss) / 2;
    os << "," << std::setprecision(prec) << (n1->m_frns + n2->m_frns) / 2;
  }
  if (m_writeWeights)
    os << "," << std::setprecision(prec) << ld->m_weight;

  return true;
}
