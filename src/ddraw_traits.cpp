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

#include "ddraw_traits.h"

const GUID ddraw_traits1::guid = IID_IDirectDraw;
const GUID ddraw_traits2::guid = IID_IDirectDraw2;
const GUID ddraw_traits4::guid = IID_IDirectDraw4;
const GUID ddraw_traits7::guid = IID_IDirectDraw7;

const GUID ddraw_traits1::surf_guid = IID_IDirectDrawSurface;
const GUID ddraw_traits2::surf_guid = IID_IDirectDrawSurface;
const GUID ddraw_traits4::surf_guid = IID_IDirectDrawSurface4;
const GUID ddraw_traits7::surf_guid = IID_IDirectDrawSurface7;
