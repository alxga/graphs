/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Graphs/libgraphs.h"
#include "Graphs/cols.h"
#include "Graphs/node.h"

#ifndef WRITERS_HEADER_FILE_INCLUDED
#define WRITERS_HEADER_FILE_INCLUDED

class LIBGRAPHS_API Graph;

class LIBGRAPHS_API NodeWriter
{
  bool m_writeCentralities;

public:  
  NodeWriter(bool wCentralities = false) : m_writeCentralities(wCentralities)
  {
  }

  virtual void writeCsvHeader(std::ostream &os);
  virtual bool writeCsv(std::ostream &os, const Node *node);
  virtual void writeGdfHeader(std::ostream &os);
  virtual bool writeGdf(std::ostream &os, const Node *node);
};

class LIBGRAPHS_API LinkWriter
{
  bool m_writeCentralities;
  bool m_writeWeights;

public:
  LinkWriter(bool wCentralities = false, bool wWeights = false) :
    m_writeCentralities(wCentralities), m_writeWeights(wWeights)
  {
  }

  virtual void writeCsvHeader(std::ostream &os);
  virtual bool writeCsv(std::ostream &os, const LinkData *link,
                        const Node *n1, const Node *n2);
  virtual void writeGdfHeader(std::ostream &os);
  virtual bool writeGdf(std::ostream &os, const LinkData *link,
                        const Node *n1, const Node *n2);
};

#endif // WRITERS_HEADER_FILE_INCLUDED
