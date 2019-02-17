/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/circgraph.h"


void CircGraph::linkNodesCircER(double b, double R)
{
  const PNodeVector &ns = nodes();
  const int nc = nCount();
  for (int i = 0; i < nc; i++)
  {
    CircNode *nd = (CircNode*)ns[i];
    nd->m_angle = 2 * M_PI * RAND_0_1;
    nd->m_coords.set(R * cos(nd->m_angle), R * sin(nd->m_angle));
  }

  for (int i = 0; i < nc - 1; i++)
    for (int j = i + 1; j < nc; j++)
    {
      CircNode *n1 = (CircNode *)ns[i];
      CircNode *n2 = (CircNode *)ns[j];
      double x = n1->angDiff(*n2) * R;
      if (1 / (1 + pow(x, b)) > RAND_0_1)
        linkSimple(n1, n2, false);
    }
}

void CircGraph::GenerateCircER(int n, double k, double b)
{
  resizeAndResetNodes(n);

  double R = n / (k * b * sin(M_PI / b));
  App::app()->log("GenerateCircER: R = %lf", R);

  linkNodesCircER(b, R);
  App::app()->log("Generated a graph with <k> = %lf", 2.0 * ldCount() / nCount());
}


double CircGraph::generateKappa(double g, double kappaMax)
{
  double ret;
  do
  {
    // (1 - RAND_0_1) assumes the RAND_0_1 range [0; 1)
    ret = exp(log(1 - RAND_0_1) / (1 - g));
  }
  while (ret > kappaMax);
  return ret;
}

void CircGraph::linkNodesCircSF(double b, double R, double g)
{
  const PNodeVector &ns = nodes();
  const int nc = nCount();
  for (int i = 0; i < nc; i++)
  {
    CircNode *nd = (CircNode*)ns[i];
    nd->m_angle = 2 * M_PI * RAND_0_1;
    nd->m_coords.set(R * cos(nd->m_angle), R * sin(nd->m_angle));
    nd->m_kappa = generateKappa(g, nc);
  }

  for (int i = 0; i < nc - 1; i++)
    for (int j = i + 1; j < nc; j++)
    {
      CircNode *n1 = (CircNode *)ns[i];
      CircNode *n2 = (CircNode *)ns[j];
      double x = n1->angDiff(*n2) * R / (n1->m_kappa * n2->m_kappa);
      if (1 / (1 + pow(x, b)) > RAND_0_1)
        linkSimple(n1, n2, false);
    }
}

void CircGraph::GenerateCircSF(int n, double k, double g, double b)
{
  resizeAndResetNodes(n);

  double t = (g - 1) / (g - 2);
  double R = n / (k * b * sin(M_PI / b)) * t * t;
  App::app()->log("GenerateCircSF: R = %lf", R);

  linkNodesCircSF(b, R, g);
}
