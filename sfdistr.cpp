/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"

#include "Graphs/sfdistr.h"
#include "Graphs/alg.h"


class IFunc
{
public:
  virtual double calc(double g) = 0;
};

class DegreeFunc : public IFunc
{
  int m_N;
  int m_max;
  double m_avK;

public:
  DegreeFunc(int N, int max, double avK) :
    m_N(N), m_max(max), m_avK(avK)
  {
  }
  virtual double calc(double g)
  {
    double numer = 0, denum = 0;
    for (int i = 1; i <= m_max; i++)
    {
      numer += pow((double) i, 1 - g);
      denum += pow((double) i, -g);
    }
    return m_avK - numer/denum;
  }
};

static double divHalf(double a, double b, IFunc *func)
{
  const double eps = 1E-5;

  double c = (a + b) / 2;
  double f = func->calc(c);

  while (std::abs(f) > eps)
  {
    if (f < 0)
      a = c;
    else
      b = c;

    c = (a + b) / 2;
    f = func->calc(c);
  }

  return c;
}

void SFDegreeDistr::initMax(double k)
{
  double numer = 0;
  double denum = 0;
  m_max = m_min;
  do
  {
    numer += pow((double) m_max, 1 - m_g);
    denum += pow((double) m_max, -m_g);
    m_max++;
    if (m_max > MAXK)
      throw Exception("Unable to initialize SF distribution");
  }
  while (numer / denum < k);
}

double SFDegreeDistr::initMin(double k)
{
  double numer = 0;
  double denum = 0;
  for (m_min = m_max; m_min >= 1; m_min--)
  {
    double pn = pow(m_min, 1 - m_g);
    double pd = pow(m_min, -m_g);
    if ((numer + pn) / (denum + pd) < k)
    {
      m_min ++;
      break;
    }
    numer += pn;
    denum += pd;
  }
  if (m_min <= 1)
    throw Exception("Unable to initialize SF distribution");

  double nd = numer / denum;
  return (nd - k) / (nd + 1 - m_min);
}

void SFDegreeDistr::initG(double k)
{
  DegreeFunc f(m_N, m_max, k);
  m_g = divHalf(2, 3, &f);
}

void SFDegreeDistr::initProbs(double x)
{
  int poolIx;
  int N = (int) (m_N * (1 - x));
  for (poolIx = m_N - 1; poolIx >= N; poolIx--)
    m_pool[poolIx] = m_min - 1;

  double norm = 0;
  for (int i = m_min; i <= m_max; i++)
    norm += pow((double) i, -m_g);

  double sumRem = 0;
  std::vector<double> rems;
  
  int minRem = m_min;
  for (int i = m_min; i <= m_max; i++)
  {
    double num = N * pow((double) i, -m_g) / norm;
    int n = (int)num;
    for (int j = 0; j < n; j++)
      m_pool[poolIx--] = i;

    if (i == m_max)
      sumRem = 1.1;
    else
      sumRem += num - n;
    rems.push_back(sumRem);
    if (sumRem > 1)
    {
      double r = RAND_0_1;
      for (int j = 0; j < (int)rems.size(); j++)
        if (rems[j] > r)
        {
          m_pool[poolIx--] = minRem + j;
          break;
        }
      sumRem --;
      rems.clear();
      rems.push_back(sumRem);
      minRem = i;
    }
  }
}


StrictSFDegreeDistr::~StrictSFDegreeDistr()
{
  delete m_selector;
}

void StrictSFDegreeDistr::initProbs(double x)
{
  SFDegreeDistr::initProbs(x);

  delete m_selector;
  m_curPool = new int *[m_N];
  m_selector = new RandomSelector<int>(m_curPool, m_N, NULL);

  reset();
}

void StrictSFDegreeDistr::reset()
{
  for (int i = 0; i < m_N; i++)
    m_curPool[i] = &m_pool[i];
  m_selector->resetCounters(m_N);
}

int StrictSFDegreeDistr::genInt()
{
  int *d = m_selector->sel();
  return d != NULL ? *d : 0;
}
