// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#include "libgraphs.h"

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#define _USE_MATH_DEFINES

#include <algorithm>
#include <numeric>
#include <cstring>
#include <string>
#include <time.h>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <queue>

#include "Utils/utils.h"

#ifdef WINDOWS
#  include "Graphs/targetver.h"
#  define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#  include <windows.h>
#endif
