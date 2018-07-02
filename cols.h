/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include <string>
#include <vector>
#include "Utils/utils.h"
#include "Graphs/libgraphs.h"

#ifndef GDFCOLS_HEADER_INCLUDED
#define GDFCOLS_HEADER_INCLUDED

class LIBGRAPHS_API CsvCol
{
  std::string *m_name;

public:
  CsvCol();
  CsvCol(const char *name);
  CsvCol(const CsvCol &v);
  virtual ~CsvCol();

  virtual void parseToken(std::string &token);
  inline void parseToken(const char *str)
  {
    std::string s(str);
    parseToken(s);
  }
  inline const std::string &name() const { return *m_name; }
};
typedef std::vector<CsvCol> CsvColVector;
std::ostream &operator<< (std::ostream &os, const CsvCol &v);


class LIBGRAPHS_API GdfCol : public CsvCol
{
public:
  typedef enum { VCh, Dbl, Int, Bool } DataType;

private:
  DataType m_type;

public:
  GdfCol();
  GdfCol(const char *name, DataType dt);
  GdfCol(const GdfCol &v);
  virtual ~GdfCol();

  virtual void parseToken(std::string &token);  
  inline void parseToken(const char *str)
  {
    std::string s(str);
    parseToken(s);
  }
  inline DataType type() const { return m_type; }
};
typedef std::vector<GdfCol> GdfColVector;


template <typename ColType>
  class GraphFileCols
  {
    std::vector<ColType> m_cols;

  public:
    GraphFileCols()
    {
    }

    virtual ~GraphFileCols()
    {
      clear();
    }

    void clear()
    {
      m_cols.clear();
    }

    void addCol(const ColType &v)
    {
      m_cols.push_back(v);
    }

    void parse(const std::string &str)
    {
      clear();

      StringList tokens;
      SplitStr(str, tokens, ",");

      for (typename StringList::iterator it = tokens.begin();
           it != tokens.end(); it++)
      {
        std::string &str = *it;

        ColType col;
        col.parseToken(str);
        if (find(col.name()) >= 0)
          throw Exception("Duplicate column name");     
        m_cols.insert(m_cols.end(), col);
      }
    }

    int find(const std::string &name)
    {
      for (size_t i = 0; i < m_cols.size(); i++)
        if (!_stricmp(m_cols[i].name().data(), name.data()))
          return (int)i;
      return -1;
    }

    inline size_t size() { return m_cols.size(); }

    virtual ColType &operator[](size_t i)
    {
      return m_cols[i];
    }
  };

typedef GraphFileCols<CsvCol> CsvCols;
typedef GraphFileCols<GdfCol> GdfCols;

#endif // GDFCOLS_HEADER_INCLUDED
