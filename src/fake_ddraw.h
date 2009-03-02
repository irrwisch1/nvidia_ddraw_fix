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

#ifndef FAKE_DDRAW_H
#define FAKE_DDRAW_H
#include <ddraw.h>
#include "compat.h"
#include "trace.h"
#include "main.h"
#include "fake_ddraw_surf.h"
#include "refcounted_wrapper.h"

template<typename T> struct ddraw_traits;

template<>
struct ddraw_traits<IDirectDraw> {
	typedef LPDDSURFACEDESC          surface_desc_ptr;
	typedef LPDIRECTDRAWSURFACE      surface_ptr;
	typedef LPDDENUMMODESCALLBACK    enum_mode_cb_ptr;
	typedef LPDDENUMSURFACESCALLBACK enum_surf_cb_ptr;
	typedef fake_ddraw_surf          fake_surf_type;
};

template<>
struct ddraw_traits<IDirectDraw2> {
	typedef LPDDSURFACEDESC          surface_desc_ptr;
	typedef LPDIRECTDRAWSURFACE      surface_ptr;
	typedef LPDDENUMMODESCALLBACK    enum_mode_cb_ptr;
	typedef LPDDENUMSURFACESCALLBACK enum_surf_cb_ptr;
	typedef fake_ddraw_surf          fake_surf_type;
};

/*! base template for all direct draw interfaces (1, 2, 4, 7)
    implements everything thats the same or similar between the
	different DDraw versions
*/
template<typename T>
struct fake_ddraw_base : public refcounted_wrapper<T> {
	typedef ddraw_traits<T> traits;

	explicit fake_ddraw_base(T* real)
		: refcounted_wrapper<T>(real) {}

	virtual ~fake_ddraw_base()
	{ LOG_STDERR( demangle(typeid(this).name()) << " deleted" ); }

	virtual HRESULT WINAPI Compact()
	{ FNTRACE; return refcounted_wrapper<T>::m_real->Compact(); }

	virtual HRESULT WINAPI CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER* b, IUnknown* c)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->CreateClipper(a, b, c); }

	virtual HRESULT WINAPI CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE* c, IUnknown* d)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->CreatePalette(a, b, c, d); }

	virtual HRESULT WINAPI CreateSurface(typename traits::surface_desc_ptr a, typename traits::surface_ptr* b, IUnknown* c)
	{
		FNTRACE
		HRESULT hr = refcounted_wrapper<T>::m_real->CreateSurface(a, b, c);
		if (FAILED(hr))
			return hr;

		typename traits::fake_surf_type* surf = new typename traits::fake_surf_type(*b);
		*b = surf;
		return hr;
	}

	virtual HRESULT WINAPI DuplicateSurface(typename traits::surface_ptr a, typename traits::surface_ptr* b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->DuplicateSurface(a, b); }

	virtual HRESULT WINAPI EnumDisplayModes(DWORD a, typename traits::surface_desc_ptr b, LPVOID c, typename traits::enum_mode_cb_ptr d)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->EnumDisplayModes(a, b, c, d); }

	virtual HRESULT WINAPI EnumSurfaces(DWORD a, typename traits::surface_desc_ptr b, LPVOID c, typename traits::enum_surf_cb_ptr d)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->EnumSurfaces(a, b, c, d); }

	virtual HRESULT WINAPI FlipToGDISurface()
	{ FNTRACE; return refcounted_wrapper<T>::m_real->FlipToGDISurface(); }

	virtual HRESULT WINAPI GetCaps(LPDDCAPS a, LPDDCAPS b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetCaps(a, b); }

	virtual HRESULT WINAPI GetDisplayMode(typename traits::surface_desc_ptr a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetDisplayMode(a); }

	virtual HRESULT WINAPI GetFourCCCodes(LPDWORD a, LPDWORD b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetFourCCCodes(a, b); }

	virtual HRESULT WINAPI GetGDISurface(typename traits::surface_ptr* a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetGDISurface(a); }

	virtual HRESULT WINAPI GetMonitorFrequency(LPDWORD a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetMonitorFrequency(a); }

	virtual HRESULT WINAPI GetScanLine(LPDWORD a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetScanLine(a); }

	virtual HRESULT WINAPI GetVerticalBlankStatus(LPBOOL a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetVerticalBlankStatus(a); }

	virtual HRESULT WINAPI Initialize(GUID* a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->Initialize(a); }

	virtual HRESULT WINAPI RestoreDisplayMode()
	{ FNTRACE; return refcounted_wrapper<T>::m_real->RestoreDisplayMode(); }

	virtual HRESULT WINAPI SetCooperativeLevel(HWND a, DWORD b)
	{
		FNTRACE
		g_hwnd = a;
		return refcounted_wrapper<T>::m_real->SetCooperativeLevel(a, b);
	}

	virtual HRESULT WINAPI WaitForVerticalBlank(DWORD a, HANDLE b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->WaitForVerticalBlank(a, b); }
};

struct fake_ddraw : public fake_ddraw_base<IDirectDraw> {
	explicit fake_ddraw(IDirectDraw* real)
		: fake_ddraw_base<IDirectDraw>(real) {}

	virtual HRESULT WINAPI QueryInterface(REFIID a, LPVOID* b);

	virtual HRESULT WINAPI SetDisplayMode(DWORD a, DWORD b, DWORD c)
	{ FNTRACE; return m_real->SetDisplayMode(a, b, c); }
};

struct fake_ddraw2 : public fake_ddraw_base<IDirectDraw2> {
	explicit fake_ddraw2(IDirectDraw2* real)
		: fake_ddraw_base<IDirectDraw2>(real) {}

	virtual HRESULT WINAPI QueryInterface(REFIID a, LPVOID* b);
	virtual HRESULT WINAPI SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e)
	{ FNTRACE; return m_real->SetDisplayMode(a, b, c, d, e); }

	//*** Added in the V2 Interface ***
	HRESULT WINAPI GetAvailableVidMem(LPDDSCAPS a, LPDWORD b, LPDWORD c)
	{ FNTRACE; return m_real->GetAvailableVidMem(a, b, c); }
};

#endif
