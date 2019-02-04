/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include <random>
#include "Utils/utils.h"
#include "Graphs/libgraphs.h"

#ifndef SFDISTR_HEADER_FILE_INCLUDED
#define SFDISTR_HEADER_FILE_INCLUDED


class LIBGRAPHS_API MKSFDegreeDistr : public IDoubleDistr
{
protected:
  int m_N;
  double m_min;
  int m_max;
  double m_g;
  double m_c;
  double m_cdfMax;
  bool m_first;
  
public:
  MKSFDegreeDistr()
  {
  }

  virtual void initialize(int N, double k, double g)
  {
    m_first = true;
    m_N = N;
    m_g = g;
    m_min = k * (m_g - 2) / (m_g - 1);
    m_c = (m_g - 1) * pow(m_min, (m_g - 1));
    m_max = (int)ceil(pow(m_N, 1 / (m_g - 1)));
    m_cdfMax = 1 + m_c * pow(m_max, (1 - m_g)) / (1 - m_g);
  }

  virtual ~MKSFDegreeDistr()
  {
  }
  
  virtual inline double genDouble()
  {
    double r = RAND_0_1;
    while (r >= m_cdfMax)
      r = RAND_0_1;
    return pow((r - 1) * (1 - m_g) / m_c, 1 / (1 - m_g));
  }
};

class LIBGRAPHS_API SFDegreeDistr : public IIntDistr
{
protected:
  int m_N;
  int m_min;
  int m_max;
  double m_g;
  int *m_pool;
  
  double initMin(double k);
  void initMax(double k);
  void initG(double k);
  virtual void initProbs(double x);

public:
  enum { MAXK = 1000 };

  SFDegreeDistr()
  {
    m_pool = NULL;
  }

  virtual void initialize(int N, double k, double g)
  {
    m_N = N;
    m_g = g;
    m_max = MAXK;
    m_pool = new int [m_N];

    double x = initMin(k);
    initProbs(x);
  }

  virtual void initialize(int N, double k, double g, int min)
  {
    m_N = N;
    m_g = g;
    m_min = min;
    m_pool = new int [m_N];

    initMax(k);
    initProbs(0);
  }

  virtual ~SFDegreeDistr()
  {
    delete [] m_pool;
  }
  
  virtual inline int genInt()
  {
    return m_pool[(int) (RAND_0_1 * m_N)];
  }
};

class LIBGRAPHS_API StrictSFDegreeDistr : public SFDegreeDistr
{
  int **m_curPool;
  RandomSelector<int> *m_selector;

public:
  StrictSFDegreeDistr()
  {
    m_selector = NULL;
  }

  virtual ~StrictSFDegreeDistr();

  virtual void initProbs(double x);

  void reset();

  virtual int genInt();
};

#endif // SFDISTR_HEADER_FILE_INCLUDED
