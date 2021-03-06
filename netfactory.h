/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Graphs/libgraphs.h"
#include "Graphs/node.h"
#include "Graphs/parsers.h"
#include "Graphs/writers.h"
#include "Graphs/blkmem.h"

#ifndef NETFACTORY_HEADER_FILE_INCLUDED
#define NETFACTORY_HEADER_FILE_INCLUDED

class INetFactory
{
public:
  virtual INetFactory *newINetFactory() = 0;
  virtual ~INetFactory() {}
  virtual Node *nextNode() = 0;
  virtual LinkData *nextLink() = 0;
  virtual void resizeNodes(int count) = 0;
  virtual void resizeLinks(int count) = 0;
};

template<typename NType, typename LType>
  class NetFactory : public INetFactory
  {
    BlockMemMgr<NType> m_nodeMMgr;
    BlockMemMgr<LType> m_linkMMgr;

  public:
    NetFactory()
    {
    }

    virtual ~NetFactory()
    {
    }

    virtual INetFactory *newINetFactory()
    {
      return new NetFactory<NType, LType>();
    }

    virtual Node *nextNode()
    {
      Node *ret = &m_nodeMMgr.next();
      ret->reinit();
      return ret;
    }

    virtual LinkData *nextLink()
    {
      LinkData *ret = &m_linkMMgr.next();
      ret->reinit();
      return ret;
    }

    virtual void resizeNodes(int count)
    {
      m_nodeMMgr.resize(count);
    }

    virtual void resizeLinks(int count)
    {
      m_linkMMgr.resize(count);
    }
  };

#endif // NETFACTORY_HEADER_FILE_INCLUDED