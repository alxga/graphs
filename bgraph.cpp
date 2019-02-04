/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/bgraph.h"


void BGraph::linkAndDeleteFromTo(BNode **from, BNode **to,
                                 int count, int numLinks)
{
  auto_del<BNode *> del_from(from, true);
  auto_del<BNode *> del_to(to, true);

  int linksRemain = numLinks;

  while (linksRemain > 0)
  {
    BNode *fn, *tn;

    do
    {
      fn = from[(int) (count * RAND_0_1)];
    }
    while (fn->dep() != NULL);

    if (fn->ctrl())
      tn = fn->ctrl();
    else
      do
      {
        tn = to[(int) (count * RAND_0_1)];
      }
      while (tn->ctrl() != NULL || tn->dep() != NULL);

    fn->setDep(tn);

    linksRemain--;

    int diff = numLinks - linksRemain;
    if ((double) diff / count > 0.01 ||
        (double) diff / RAND_MAX > 0.3)
    {
      int fix = 0, tix = 0;
      BNode **nfrom = new BNode *[count - diff];
      BNode **nto = new BNode *[count - diff];
      for (int i = 0; i < count; i++)
      {
        if (from[i]->dep() == NULL)
          nfrom[fix++] = from[i];
        if (!to[i]->ctrl())
          nto[tix++] = to[i];
      }
      delete [] from;
      delete [] to;
      from = nfrom;
      del_from.setPtr(from);
      to = nto;
      del_to.setPtr(to);
      numLinks -= diff;
      count -= diff;
    }
  }
}

int BGraph::DeactDependent(int dactTime)
{
  const size_t count = nCount();
  const PNodeVector &nv = nodes();
  int ret = 0;

  for (size_t i = 0; i < count; i++)
  {
    BNode &n = *((BNode *) nv[i]);
    if (n.m_dactTime < 0)
    {
      if (!n.controlOk())
        n.m_dactTime = dactTime;
      else
        ret++;
    }
  }

  return ret;
}

void BGraph::ReadDependencyLinks(const char *path, BGraph &g, bool isMutual)
{
  std::ifstream difs(path, std::ios_base::in | std::ios_base::binary);

  if (!difs.good())
    throw new Exception("Unable to open a dependencies input file");
  
  std::string str;
  while (std::getline(difs, str))
  {
    StringVector strs;
    SplitStr(str, strs, " ");
    if (strs.size() != 2)
      throw new Exception("Incorrect line in a dependencies input file");

    BNode *n1 = (BNode *) nodeMap()[strs[0]];
    BNode *n2 = (BNode *) g.nodeMap()[strs[1]];
    n1->setDep(n2);
    if (isMutual)
      n2->setDep(n1);
  }
}

void BGraph::WriteDependencyLinks(const char *path)
{
  std::ofstream difs(path, std::ios_base::out | std::ios_base::binary);

  const int C = nCount();
  const PNodeVector &nv = nodes();
  for (int i = 0; i < C; i++)
  {
    BNode *dep = ((BNode *) nv[i])->dep();
    if (dep != NULL)
      difs << nv[i]->m_name << " " << dep->m_name << CSVENDL;
  }
}


void BGraph::GenerateCoupling(Graph &g, double q)
{
  const int nc = nCount();

  BNode **from = new BNode *[nc];
  BNode **to = new BNode *[nc];

  for (int i = 0; i < nc; i++)
  {
    from[i] = (BNode *) nodes()[i];
    to[i] = (BNode *) g.nodes()[i];
  }

  linkAndDeleteFromTo(from, to, nc, (int) (q * nc));
}
