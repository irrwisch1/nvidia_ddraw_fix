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

#ifndef TRACE_H
#define TRACE_H

#if defined(_DEBUG) || defined(NVIDIA_DDRAW_FIX_LOGGING)
#include <fstream>
#include <iostream>
extern std::ofstream outstream;

#define LOG(X) outstream << X << std::endl;
#endif

//#define outstream std::cout
#ifdef _DEBUG
	//#define LOG_STDERR(X) std::cerr << X << std::endl;
	#define LOG_STDERR(X) outstream << X << std::endl;
	#define LOG_FILE(X) outstream << X << std::endl;

	/*
	#define FNTRACE \
		struct fntrace { \
			fntrace(const char* fun) : fun(fun) {} \
			~fntrace() { stream << "<- " << fun << std::endl; } \
			const char* fun; \
		} fntrace(__FUNCSIG__); \
		stream << "-> " << __FUNCSIG__ << std::endl;
	*/

	#if defined(__GNUC__)
		#define FUNCTION_NAME __PRETTY_FUNCTION__
	#elif defined (_MSC_VER)
		#define FUNCTION_NAME __FUNCSIG__
	#else
		#define FUNCTION_NAME __FUNCTION__
	#endif

	#define FNTRACE outstream << FUNCTION_NAME << std::endl;
#else
	#define LOG_STDERR(X)
	#define LOG_FILE(X)
	#define FNTRACE
#endif

#endif
