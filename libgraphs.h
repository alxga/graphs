/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#ifndef LIBGRAPHS_HEADER_FILE_INCLUDED
#define LIBGRAPHS_HEADER_FILE_INCLUDED

#if defined(WIN32) || defined(WIN64)
#  define WINDOWS
#endif

#ifdef WINDOWS
#  ifdef LIBGRAPHS_EXPORTS
#    define LIBGRAPHS_API __declspec(dllexport)
#  else
#    define LIBGRAPHS_API __declspec(dllimport)
#  endif
#else
#  define LIBGRAPHS_API
#endif

#endif // LIBGRAPHS_HEADER_FILE_INCLUDED