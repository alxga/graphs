/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/cols.h"


CsvCol::CsvCol()
{
  m_name = new std::string();
}

CsvCol::CsvCol(const char *name)
{
  m_name = new std::string(name);
}

CsvCol::CsvCol(const CsvCol &v)
{
  m_name = new std::string(v.name());
}

CsvCol::~CsvCol()
{
  delete m_name;
}

void CsvCol::parseToken(std::string &token)
{
  Trim(token);
  if (token.length() <= 0)
    throw Exception("Invalid column name");
  *m_name = token;
}

std::ostream &operator<< (std::ostream &os, const CsvCol &v)
{
  os << v.name();
  return os;
}


GdfCol::GdfCol() : m_type(GdfCol::VCh)
{
}

GdfCol::GdfCol(const char *name, GdfCol::DataType dt) : CsvCol(name)
{
  m_type = dt;
}

GdfCol::GdfCol(const GdfCol &v) : CsvCol(v)
{
  m_type = v.m_type;
}

GdfCol::~GdfCol()
{
}

void GdfCol::parseToken(std::string &token)
{
  StringVector strs;
  SplitStr(token, strs, " ");
  if (strs.size() == 1)
  {
    m_type = VCh;
    CsvCol::parseToken(strs[0]);
  }
  else if (strs.size() == 2)
  {
    Trim(strs[1]);
    if (_stricmp(strs[1].data(), "DOUBLE") == 0)
      m_type = Dbl;
    else if (_stricmp(strs[1].data(), "INTEGER") == 0)
      m_type = Int;
    else if (_stricmp(strs[1].data(), "BOOLEAN") == 0)
      m_type = Bool;
    else
      m_type = VCh;
    CsvCol::parseToken(strs[0]);
  }
  else
    throw Exception("More than 2 GDF column tokens");
}
