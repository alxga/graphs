/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Graphs/writers.h"

#ifndef CIRCNODEWRITER_HEADER_FILE_INCLUDED
#define CIRCNODEWRITER_HEADER_FILE_INCLUDED

class LIBGRAPHS_API CircNodeWriter : public NodeWriter
{
public:
  virtual void writeCsvHeader(std::ostream &os);
  virtual bool writeCsv(std::ostream &os, const Node *node);
};

#endif // CIRCNODEWRITER_HEADER_FILE_INCLUDED
