/*
	nVidia DirectDraw Fix
	Copyright (c) 2009 Julien Langer

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NVIDIA_DDRAW_FIX_STRNCASECMP_H
#define NVIDIA_DDRAW_FIX_STRNCASECMP_H

#include "config.h"

#if defined(HAVE_STRNCASECMP)
	#include <strings.h>
#elif defined(HAVE_STRNICMP)
	#include <string.h>
	inline int strncasecmp(const char *s1, const char *s2, size_t n)
	{
		#ifdef _MSC_VER
		return _strnicmp(s1, s2, n);
		#else
		return strnicmp(s1, s2, n);
		#endif
	}
#else
	#error I don't have strncasecmp and strnicmp..don't know what to do now
#endif

#endif
