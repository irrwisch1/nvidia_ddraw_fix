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
#include "ddraw_traits.h"
#include "main.h"
#include "fake_ddraw_surf.h"
#include "refcounted_wrapper.h"

struct fake_ddraw;
struct fake_ddraw2;
struct fake_ddraw4;
struct fake_ddraw7;

#define DO_STRINGIZE(X) #X
#define STRINGIZE(X) DO_STRINGIZE(X)

// some macro magic to safe some repetetive typing
#define QUERY_INTERFACE_IMPL(NUM) \
	LOG_STDERR("requesting ddraw" STRINGIZE(NUM) ) \
	HRESULT hr = m_real->QueryInterface( IID_IDirectDraw##NUM, obp ); \
	\
	if ( *obp == NULL ) { \
		LOG_STDERR("could not retrieve ddraw " STRINGIZE(NUM) "interface"); \
		return hr; \
	} \
	\
	*obp = new fake_ddraw##NUM( static_cast<IDirectDraw##NUM*>(*obp) ); \
	return DD_OK; \

/*! base template for all direct draw interfaces (1, 2, 4, 7)
    implements everything thats the same or similar between the
	different DDraw versions
*/
template<typename T>
struct fake_ddraw_base : public refcounted_wrapper<T> {
	typedef typename ddraw_traits_selector<T>::traits traits;

	explicit fake_ddraw_base(T* real)
		: refcounted_wrapper<T>(real) { LOG_STDERR( demangle(typeid(this).name()) << " created" ) }

	virtual ~fake_ddraw_base()
	{ LOG_STDERR( demangle(typeid(this).name()) << " deleted" ); }

	virtual HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* obp)
	{
		FNTRACE

		if ( traits::guid == riid ) {
			*obp = this;
			AddRef();
			return DD_OK;
		}
		else if ( riid == IID_IDirectDraw ) {
			LOG_STDERR("requesting ddraw 1")
			return create_interface<IDirectDraw>(obp);
		}
		else if ( riid == IID_IDirectDraw2 ) {
			LOG_STDERR("requesting ddraw 2")
			return create_interface<IDirectDraw2>(obp);
		}
		else if ( riid == IID_IDirectDraw4 ) { 
			LOG_STDERR("requesting ddraw 4")
			return create_interface<IDirectDraw4>(obp);
		}
		else if ( riid == IID_IDirectDraw7 ) {
			LOG_STDERR("requesting ddraw 7")
			return create_interface<IDirectDraw7>(obp);
		}
		else {
			show_error_box("unknown direct draw interface requested", MB_OK | MB_ICONERROR );
			return DD_FALSE;
		}
	}

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

	private:
		template<typename Interface>
		HRESULT WINAPI create_interface(LPVOID* obp)
		{
			typedef typename ddraw_traits_selector<Interface>::traits q_traits;
			HRESULT hr = m_real->QueryInterface( q_traits::guid, obp );

			if ( *obp == NULL ) {
				LOG_STDERR("could not retrieve ddraw interface");
				return hr;
			}

			*obp = new typename q_traits::fake_ddraw_type( static_cast<Interface*>(*obp) );
			return DD_OK;
		}
};

/*! base for all direct draw interfaces >= 2 */
template<typename T>
struct fake_ddraw2_base : public fake_ddraw_base<T> {
	typedef typename ddraw_traits_selector<T>::traits traits;

	explicit fake_ddraw2_base(T* real)
		: fake_ddraw_base<T>(real) {}

	virtual HRESULT WINAPI SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e)
	{ FNTRACE; return m_real->SetDisplayMode(a, b, c, d, e); }

	virtual HRESULT WINAPI GetAvailableVidMem(typename traits::caps_ptr a, LPDWORD b, LPDWORD c)
	{ FNTRACE; return m_real->GetAvailableVidMem(a, b, c); }
};

/*! base for all direct draw interfaces >= 4 */
template<typename T>
struct fake_ddraw4_base : public fake_ddraw2_base<T> {
	typedef typename ddraw_traits_selector<T>::traits traits;

	explicit fake_ddraw4_base(T* real)
		: fake_ddraw2_base<T>(real) {}

	virtual HRESULT WINAPI GetSurfaceFromDC(HDC a, typename traits::surface_ptr* b)
	{ FNTRACE; return m_real->GetSurfaceFromDC(a, b); }

    virtual HRESULT WINAPI RestoreAllSurfaces()
	{ FNTRACE; return m_real->RestoreAllSurfaces(); }

    virtual HRESULT WINAPI TestCooperativeLevel()
	{ FNTRACE; return m_real->TestCooperativeLevel(); }

	virtual HRESULT WINAPI GetDeviceIdentifier(typename traits::device_identifier_ptr a, DWORD b)
	{ FNTRACE; return m_real->GetDeviceIdentifier(a, b); }
};

/*! concrete implementation for IDirectDraw interface */
struct fake_ddraw : public fake_ddraw_base<IDirectDraw> {
	explicit fake_ddraw(IDirectDraw* real)
		: fake_ddraw_base<IDirectDraw>(real) {}

	virtual HRESULT WINAPI SetDisplayMode(DWORD a, DWORD b, DWORD c)
	{ FNTRACE; return m_real->SetDisplayMode(a, b, c); }
};

/*! concrete implementation for IDirectDraw2 interface */
struct fake_ddraw2 : public fake_ddraw2_base<IDirectDraw2> {
	explicit fake_ddraw2(IDirectDraw2* real)
		: fake_ddraw2_base<IDirectDraw2>(real) {}
};

/*! concrete implementation for IDirectDraw4 interface */
struct fake_ddraw4 : public fake_ddraw4_base<IDirectDraw4> {
	explicit fake_ddraw4(IDirectDraw4* real)
		: fake_ddraw4_base<IDirectDraw4>(real) {}
};

/*! concrete implementation for IDirectDraw7 interface */
struct fake_ddraw7 : public fake_ddraw4_base<IDirectDraw7> {
	explicit fake_ddraw7(IDirectDraw7* real)
		: fake_ddraw4_base<IDirectDraw7>(real) {}

    virtual HRESULT WINAPI StartModeTest(LPSIZE a, DWORD b, DWORD c)
	{ FNTRACE; return m_real->StartModeTest(a, b, c); }

    virtual HRESULT WINAPI EvaluateMode(DWORD a, DWORD *b)
	{ FNTRACE; return m_real->EvaluateMode(a, b); }
};

#endif
