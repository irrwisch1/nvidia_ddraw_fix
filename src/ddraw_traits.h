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

#ifndef NVIDIA_DDRAW_FIX_DDRAW_TRAITS_H
#define NVIDIA_DDRAW_FIX_DDRAW_TRAITS_H

#include <ddraw.h>

struct fake_ddraw_surf;
struct fake_ddraw_surf4;
struct fake_ddraw_surf7;
struct fake_ddraw;
struct fake_ddraw2;
struct fake_ddraw4;
struct fake_ddraw7;

struct ddraw_traits1 {
	typedef DDSURFACEDESC            surface_desc;
	typedef LPDDSURFACEDESC          surface_desc_ptr;
	typedef LPDIRECTDRAWSURFACE      surface_ptr;
	typedef LPDDENUMMODESCALLBACK    enum_mode_cb_ptr;
	typedef LPDDENUMSURFACESCALLBACK enum_surf_cb_ptr;
	typedef fake_ddraw_surf          fake_surf_type;
	typedef fake_ddraw               fake_ddraw_type;
	typedef LPDDSCAPS                caps_ptr;
	static const GUID                guid;
	static const GUID                surf_guid;

	/*! type used as first argument to IDirectDrawSurface::Unlock
	    is different between version 1 and all other versions
	*/
	typedef LPVOID                   unlock_type;
};

// same as version 1 except GUID
struct ddraw_traits2 {
	typedef DDSURFACEDESC            surface_desc;
	typedef LPDDSURFACEDESC          surface_desc_ptr;
	typedef LPDIRECTDRAWSURFACE      surface_ptr;
	typedef LPDDENUMMODESCALLBACK    enum_mode_cb_ptr;
	typedef LPDDENUMSURFACESCALLBACK enum_surf_cb_ptr;
	typedef fake_ddraw_surf          fake_surf_type;
	typedef fake_ddraw2              fake_ddraw_type;
	typedef LPDDSCAPS                caps_ptr;
	static const GUID                guid;
	static const GUID                surf_guid;

	/*! type used as first argument to IDirectDrawSurface::Unlock
	    is different between version 1 and all other versions
	*/
	typedef LPVOID                   unlock_type;
};


struct ddraw_traits4 {
	typedef DDSURFACEDESC2            surface_desc;
	typedef LPDDSURFACEDESC2          surface_desc_ptr;
	typedef LPDIRECTDRAWSURFACE4      surface_ptr;
	typedef LPDDENUMMODESCALLBACK2    enum_mode_cb_ptr;
	typedef LPDDENUMSURFACESCALLBACK2 enum_surf_cb_ptr;
	typedef fake_ddraw_surf4          fake_surf_type;
	typedef fake_ddraw4               fake_ddraw_type;
	typedef LPDDSCAPS2                caps_ptr;
	typedef LPDDDEVICEIDENTIFIER      device_identifier_ptr;
	static const GUID                 guid;
	static const GUID                 surf_guid;

	/*! type used as first argument to IDirectDrawSurface::Unlock
	    is different between version 1 and all other versions
	*/
	typedef LPRECT                    unlock_type;
};

struct ddraw_traits7 {
	typedef DDSURFACEDESC2            surface_desc;
	typedef LPDDSURFACEDESC2          surface_desc_ptr;
	typedef LPDIRECTDRAWSURFACE7      surface_ptr;
	typedef LPDDENUMMODESCALLBACK2    enum_mode_cb_ptr;
	typedef LPDDENUMSURFACESCALLBACK7 enum_surf_cb_ptr;
	typedef fake_ddraw_surf7          fake_surf_type;
	typedef fake_ddraw7               fake_ddraw_type;
	typedef LPDDSCAPS2                caps_ptr;
	typedef LPDDDEVICEIDENTIFIER2     device_identifier_ptr;
	static const GUID                 guid;
	static const GUID                 surf_guid;

	/*! type used as first argument to IDirectDrawSurface::Unlock
	    is different between version 1 and all other versions
	*/
	typedef LPRECT                    unlock_type;
};

// helpers to select the right traits
template<typename T> struct ddraw_traits_selector;

template<>
struct ddraw_traits_selector<IDirectDraw> {
	typedef ddraw_traits1 traits;
};

template<>
struct ddraw_traits_selector<IDirectDraw2> {
	typedef ddraw_traits2 traits;
};

template<>
struct ddraw_traits_selector<IDirectDraw4> {
	typedef ddraw_traits4 traits;
};

template<>
struct ddraw_traits_selector<IDirectDraw7> {
	typedef ddraw_traits7 traits;
};

template<>
struct ddraw_traits_selector<IDirectDrawSurface> {
	typedef ddraw_traits1 traits;
};

template<>
struct ddraw_traits_selector<IDirectDrawSurface4> {
	typedef ddraw_traits4 traits;
};

template<>
struct ddraw_traits_selector<IDirectDrawSurface7> {
	typedef ddraw_traits7 traits;
};

#endif
