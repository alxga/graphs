/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/utils.h"

#ifndef BLKMEM_HEADER_FILE
#define BLKMEM_HEADER_FILE

template<typename T>
  class BlockMemMgr
  {
    size_t m_blkSize;
    size_t m_count;

  public:
    std::vector<T *> m_blocks;
    T &next()
    {
      if (m_count % m_blkSize == 0)
      {
        size_t nsz = m_count / m_blkSize + 1;
        if (nsz > m_blocks.size())
          m_blocks.push_back(new T[m_blkSize]);
        else
        {
          delete [] m_blocks[nsz - 1];
          m_blocks[nsz - 1] = new T[m_blkSize];
        }
      }
      T &ret = m_blocks[m_count / m_blkSize][m_count % m_blkSize];
      m_count++;
      return ret;
    }
    T &operator[](int i)
    {
      return m_blocks[i / m_blkSize][i % m_blkSize];
    }
    inline void clear() { m_count = 0; }
    inline size_t count() const { return m_count; }

    BlockMemMgr(size_t blkSize = 1024) : m_count(0), m_blkSize(blkSize)
    {
    }

    virtual ~BlockMemMgr()
    {
      for (size_t i = 0; i < m_blocks.size(); i++)
        delete [] m_blocks[i];
    }
  };

#endif // BLKMEM_HEADER_FILE
