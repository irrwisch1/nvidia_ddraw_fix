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

#ifndef NV_DDRAW_FIX_MAIN_H
#define NV_DDRAW_FIX_MAIN_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

/*! handle to the window of the game. will be initialized
    when fake_ddraw::SetCooperativeLevel os called the first time
*/
extern HWND g_hwnd;

/*! \brief shows a message box and quits the program
    \param message message
	\param flags flags as in WinAPI's MessageBox function
*/
extern void show_error_box(const char* message, uint32_t flags);

/*! demangles a mangeled name */
extern std::string demangle(const char* name);

#endif
