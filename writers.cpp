/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/writers.h"


void NodeWriter::writeCsvHeader(std::ostream &os)
{
  os << "name";
  os << "," << "btws";
  os << "," << "clss";
  os << "," << "lat";
  os << "," << "lng";
}

bool NodeWriter::writeCsv(std::ostream &os, const Node *node)
{
  const int prec = 10;

  os << node->m_name;
  os << "," << std::setprecision(prec) << node->m_btws;
  os << "," << std::setprecision(prec) << node->m_clss;
  os << "," << std::setprecision(prec) << node->lat();
  os << "," << std::setprecision(prec) << node->lng();

  return true;
}

void NodeWriter::writeGdfHeader(std::ostream &os)
{
  os << "nodedef> ";

  os << "name VARCHAR";
  os << "," << "btws DOUBLE";
  os << "," << "clss DOUBLE";
  os << "," << "lat DOUBLE";
  os << "," << "lng DOUBLE";
}

bool NodeWriter::writeGdf(std::ostream &os, const Node *node)
{
  const int prec = 10;

  os << node->m_name;
  os << "," << std::setprecision(prec) << node->m_btws;
  os << "," << std::setprecision(prec) << node->m_clss;
  os << "," << std::setprecision(prec) << node->lat();
  os << "," << std::setprecision(prec) << node->lng();

  return true;
}


void LinkWriter::writeCsvHeader(std::ostream &os)
{
  os << "node1";
  os << "," << "node2";
  os << "," << "directed";
  os << "," << "length";
  os << "," << "btws";
  os << "," << "clss";
}

bool LinkWriter::writeCsv(std::ostream &os, const LinkData *ld,
                          const Node *n1, const Node *n2)
{
  const int prec = 10;

  os << n1->m_name;
  os << "," << n2->m_name;
  os << "," << (ld->m_directed ? "true" : "false");
  os << "," << std::setprecision(prec) << ld->m_length;
  os << "," << std::setprecision(prec) << ld->m_btws;
  os << "," << std::setprecision(prec) << ld->m_clss;

  return true;
}

void LinkWriter::writeGdfHeader(std::ostream &os)
{
  os << "edgedef> ";

  os << "node1 VARCHAR";
  os << "," << "node2 VARCHAR";
  os << "," << "directed BOOLEAN";
  os << "," << "length DOUBLE";
  os << "," << "btws DOUBLE";
  os << "," << "clss DOUBLE";
}

bool LinkWriter::writeGdf(std::ostream &os, const LinkData *ld,
                          const Node *n1, const Node *n2)
{
  const int prec = 10;

  os << n1->m_name;
  os << "," << n2->m_name;
  os << "," << (ld->m_directed ? "true" : "false");
  os << "," << std::setprecision(prec) << ld->m_length;
  os << "," << std::setprecision(prec) << ld->m_btws;
  os << "," << std::setprecision(prec) << ld->m_clss;

  return true;
}
