/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Graphs/link.h"

const LinkData &LinkData::operator= (const LinkData &v)
{
  memcpy(this, &v, sizeof(LinkData));
  return *this;
}

