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

#ifndef FAKE_DDRAW_SURF_H
#define FAKE_DDRAW_SURF_H

#include <ddraw.h>
#include <sstream>
#include "refcounted_wrapper.h"
#include "trace.h"
#include "ddraw_traits.h"
#include "compat.h"

// Note: I really wonder where IDirectDrawSurface2 and 3 are used

template<typename T>
void fill(T* ptr, const RECT& rect, T val, unsigned pitch)
{
	const unsigned p = pitch / sizeof(T);

	for (int i = 0; i < rect.bottom - rect.top; ++i)
		//for ( int j = 0; j < rect.right - rect.left; ++j)
		//	ptr[i*p+j] = val;
		std::fill( &ptr[i * p], &ptr[i * p + rect.right] - rect.left, val );
}

#pragma pack(push, 1)
template<size_t N>
struct pix_packed {
	uint8_t data[N / 8];
};
#pragma pack(pop)

template<size_t N>
void generic_fill(void* ptr, const RECT& rect, uint32_t fill_val, unsigned pitch)
{
	pix_packed<N>* p = reinterpret_cast<pix_packed<N>*>(ptr);

	// some type punning
	union {
		pix_packed<N> as_packed;
		uint32_t      as_32;
	} fill_col;

	fill_col.as_32 = fill_val;

	fill(p, rect, fill_col.as_packed, pitch);
}

/*! basic template for fake ddraw surface interfaces
    \tparam T COM interface to wrap
	\tparam U type of derived class
*/
template<typename T, typename U>
struct fake_ddraw_surf_base : public refcounted_wrapper<T> {
	typedef typename ddraw_traits_selector<T>::traits traits;

	explicit fake_ddraw_surf_base(T* real)
		: refcounted_wrapper<T>(real) {}

	virtual ~fake_ddraw_surf_base()
	{ /* LOG_STDERR( demangle(typeid(this).name()) << " deleted" );*/ }

	virtual HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* obp)
	{
		FNTRACE

		if ( traits::surf_guid == riid ) {
			*obp = this;
			refcounted_wrapper<T>::AddRef();
			return DD_OK;
		}
		else if ( riid == IID_IDirectDrawSurface ) {
			LOG_STDERR("requesting ddraw 1")
			return create_interface<IDirectDrawSurface>(obp);
		}
		else if ( riid == IID_IDirectDraw4 ) {
			LOG_STDERR("requesting ddraw 4")
			return create_interface<IDirectDrawSurface4>(obp);
		}
		else if ( riid == IID_IDirectDraw7 ) {
			LOG_STDERR("requesting ddraw 7")
			return create_interface<IDirectDrawSurface7>(obp);
		}
		else {
			show_error_box("unknown direct draw surfacae interface requested", MB_OK | MB_ICONERROR );
			return DD_FALSE;
		}
	}

	virtual HRESULT WINAPI AddAttachedSurface(typename traits::surface_ptr a)
	{
		FNTRACE
		if ( fake_ddraw_surf_base* surf = dynamic_cast<fake_ddraw_surf_base*>(a) )
			return refcounted_wrapper<T>::m_real->AddAttachedSurface(surf->m_real);
		else
			return refcounted_wrapper<T>::m_real->AddAttachedSurface(a);
	}

	virtual HRESULT WINAPI AddOverlayDirtyRect(LPRECT a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->AddOverlayDirtyRect(a); }

	virtual HRESULT WINAPI Blt(LPRECT dest_rect, typename traits::surface_ptr src_surf, LPRECT src_rect, DWORD flags, LPDDBLTFX blt_fx)
	{
		FNTRACE

		// Note: the bug doesn't seem to happen with blits on the full surface (dest_rect == NULL)
		// so as a small performance optimization we skip those blits
		if ( dest_rect && flags & DDBLT_COLORFILL ) {

			typename traits::surface_desc surf_desc;
			surf_desc.dwSize = sizeof(typename traits::surface_desc);

			HRESULT res = refcounted_wrapper<T>::m_real->Lock(dest_rect, &surf_desc, DDLOCK_WRITEONLY |DDLOCK_SURFACEMEMORYPTR, NULL);

			if ( FAILED(res) ) {
				const char* result;
				switch ( res ) {
					case DDERR_INVALIDOBJECT:   result = "DDERR_INVALIDOBJECT";   break;
					case DDERR_INVALIDPARAMS:   result = "DDERR_INVALIDPARAMS";   break;
					case DDERR_OUTOFMEMORY:     result = "DDERR_OUTOFMEMORY";     break;
					case DDERR_SURFACEBUSY:     result = "DDERR_SURFACEBUSY";     break;
					case DDERR_SURFACELOST:     result = "DDERR_SURFACELOST";     break;
					case DDERR_WASSTILLDRAWING: result = "DDERR_WASSTILLDRAWING"; break;
					default: result = "unknown error";
				}

				LOG_STDERR("failed to lock surface: " << result );
				return res;
			}
			else {
				#ifdef NVIDIA_DDRAW_FIX_PROFILE
				LARGE_INTEGER t1, t2, fr;
				QueryPerformanceCounter(&t1);
				QueryPerformanceFrequency(&fr);
				#endif

				RECT surf_rect;
				if ( !dest_rect ) {
					surf_rect.top    = 0;
					surf_rect.left   = 0;
					surf_rect.right  = surf_desc.dwWidth;
					surf_rect.bottom = surf_desc.dwHeight;
				}

				const RECT& r = dest_rect ? *dest_rect : surf_rect;
				void* const p = surf_desc.lpSurface;
				const DWORD f = blt_fx->dwFillColor;
				const long  s = surf_desc.lPitch;

				switch ( surf_desc.ddpfPixelFormat.dwRGBBitCount ) {
					case 16: generic_fill<16>(p, r, f, s); break;
					case 24: generic_fill<24>(p, r, f, s); break;
					case 32: generic_fill<32>(p, r, f, s); break;

					default: {
						std::stringstream str;
						str << "Unsupported bit depth: " << surf_desc.ddpfPixelFormat.dwRGBBitCount;
						show_error_box( str.str().c_str(), MB_OK | MB_ICONERROR );
					}
				}

				#ifdef NVIDIA_DDRAW_FIX_PROFILE
				QueryPerformanceCounter(&t2);

				static bool once = true;
				if (once) {
					std::cout << "time: " << t2.QuadPart - t1.QuadPart << std::endl;
					once = false;
				}
				#endif

				refcounted_wrapper<T>::m_real->Unlock(NULL);
				return DD_OK;
			}
		}

		if ( fake_ddraw_surf_base* surf = dynamic_cast<fake_ddraw_surf_base*>(src_surf) )
			return refcounted_wrapper<T>::m_real->Blt(dest_rect, surf->m_real, src_rect, flags, blt_fx);
		else
			return refcounted_wrapper<T>::m_real->Blt(dest_rect, src_surf, src_rect, flags, blt_fx);
	}

	virtual HRESULT WINAPI BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->BltBatch(a, b, c); }

	virtual HRESULT WINAPI BltFast(DWORD x, DWORD y, typename traits::surface_ptr src_surf, LPRECT src_rect, DWORD trans)
	{
		FNTRACE

		if ( fake_ddraw_surf_base* surf = dynamic_cast<fake_ddraw_surf_base*>(src_surf) )
			return refcounted_wrapper<T>::m_real->BltFast(x, y, surf->m_real, src_rect, trans);
		else
			return refcounted_wrapper<T>::m_real->BltFast(x, y, src_surf, src_rect, trans);
	}

	virtual HRESULT WINAPI DeleteAttachedSurface(DWORD a, typename traits::surface_ptr b)
	{
		if ( fake_ddraw_surf_base* surf = dynamic_cast<fake_ddraw_surf_base*>(b) )
			return refcounted_wrapper<T>::m_real->DeleteAttachedSurface(a, surf->m_real);
		else
			return refcounted_wrapper<T>::m_real->DeleteAttachedSurface(a, b);
	}

	virtual HRESULT WINAPI EnumAttachedSurfaces(LPVOID a, typename traits::enum_surf_cb_ptr b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->EnumAttachedSurfaces(a, b); }

	virtual HRESULT WINAPI EnumOverlayZOrders(DWORD a, LPVOID b, typename traits::enum_surf_cb_ptr c)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->EnumOverlayZOrders(a, b, c); }

	virtual HRESULT WINAPI Flip(typename traits::surface_ptr a, DWORD b)
	{
		FNTRACE
		if ( fake_ddraw_surf_base* surf = dynamic_cast<fake_ddraw_surf_base*>(a) )
			return refcounted_wrapper<T>::m_real->Flip(surf->m_real, b);
		else
			return refcounted_wrapper<T>::m_real->Flip(a, b);
	}

	virtual HRESULT WINAPI GetAttachedSurface(typename traits::caps_ptr a, typename traits::surface_ptr * b)
	{
		FNTRACE
		HRESULT res = refcounted_wrapper<T>::m_real->GetAttachedSurface(a, b);
		// It's important that we return a pointer to a wrapped COM object here,
		// so we need to wrap the original object in our wrapped COM object
		// This is the only place we need the type U of the derived class
		*b = new U(*b);
		return res;
	}

	virtual HRESULT WINAPI GetBltStatus(DWORD a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetBltStatus(a); }

	virtual HRESULT WINAPI GetCaps(typename traits::caps_ptr a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetCaps(a); }

	virtual HRESULT WINAPI GetClipper(LPDIRECTDRAWCLIPPER* a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetClipper(a); }

	virtual HRESULT WINAPI GetColorKey(DWORD a, LPDDCOLORKEY b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetColorKey(a, b); }

	virtual HRESULT WINAPI GetDC(HDC* a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetDC(a); }

	virtual HRESULT WINAPI GetFlipStatus(DWORD a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetFlipStatus(a); }

	virtual HRESULT WINAPI GetOverlayPosition(LPLONG a, LPLONG b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetOverlayPosition(a, b); }

	virtual HRESULT WINAPI GetPalette(LPDIRECTDRAWPALETTE* a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetPalette(a); }

	virtual HRESULT WINAPI GetPixelFormat(LPDDPIXELFORMAT a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetPixelFormat(a); }

	virtual HRESULT WINAPI GetSurfaceDesc(typename traits::surface_desc_ptr a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetSurfaceDesc(a); }

	virtual HRESULT WINAPI Initialize(LPDIRECTDRAW a, typename traits::surface_desc_ptr b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->Initialize(a, b); }

	virtual HRESULT WINAPI IsLost()
	{ FNTRACE; return refcounted_wrapper<T>::m_real->IsLost(); }

	virtual HRESULT WINAPI Lock(LPRECT a, typename traits::surface_desc_ptr b, DWORD c, HANDLE d)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->Lock(a, b, c, d); }

	virtual HRESULT WINAPI ReleaseDC(HDC a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->ReleaseDC(a); }

	virtual HRESULT WINAPI Restore()
	{ FNTRACE; return refcounted_wrapper<T>::m_real->Restore(); }

	virtual HRESULT WINAPI SetClipper(LPDIRECTDRAWCLIPPER a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->SetClipper(a); }

	virtual HRESULT WINAPI SetColorKey(DWORD a, LPDDCOLORKEY b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->SetColorKey(a, b); }

	virtual HRESULT WINAPI SetOverlayPosition(LONG a, LONG b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->SetOverlayPosition(a, b); };

	virtual HRESULT WINAPI SetPalette(LPDIRECTDRAWPALETTE a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->SetPalette(a); }

	virtual HRESULT WINAPI Unlock(typename traits::unlock_type a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->Unlock(a); }

	virtual HRESULT WINAPI UpdateOverlay(LPRECT a, typename traits::surface_ptr b, LPRECT c, DWORD d, LPDDOVERLAYFX e)
	{
		FNTRACE
		if ( fake_ddraw_surf_base* surf = dynamic_cast<fake_ddraw_surf_base*>(b) )
			return refcounted_wrapper<T>::m_real->UpdateOverlay(a, surf->m_real, c, d, e);
		else
			return refcounted_wrapper<T>::m_real->UpdateOverlay(a, b, c, d, e);
	}

	virtual HRESULT WINAPI UpdateOverlayDisplay(DWORD a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->UpdateOverlayDisplay(a); }

	virtual HRESULT WINAPI UpdateOverlayZOrder(DWORD a, typename traits::surface_ptr b)
	{
		FNTRACE
		if ( fake_ddraw_surf_base* surf = dynamic_cast<fake_ddraw_surf_base*>(b) )
			return refcounted_wrapper<T>::m_real->UpdateOverlayZOrder(a, surf->m_real);
		else
			return refcounted_wrapper<T>::m_real->UpdateOverlayZOrder(a, b);
	}

	private:
		template<typename Interface>
		HRESULT WINAPI create_interface(LPVOID* obp)
		{
			typedef typename ddraw_traits_selector<Interface>::traits q_traits;
			HRESULT hr = refcounted_wrapper<T>::m_real->QueryInterface( q_traits::surf_guid, obp );

			if ( *obp == NULL ) {
				LOG_STDERR("could not retrieve ddraw surface interface");
				return hr;
			}

			*obp = new typename q_traits::fake_surf_type( static_cast<Interface*>(*obp) );
			return DD_OK;
		}
};

//! base for IDirectDrawSurface interfaces >= 4
template<typename T, typename U>
struct fake_ddraw4_surf_base : public fake_ddraw_surf_base<T,U> {
	typedef typename ddraw_traits_selector<T>::traits traits;

	explicit fake_ddraw4_surf_base(T* real)
		: fake_ddraw_surf_base<T, U>(real) {}

	virtual HRESULT WINAPI GetDDInterface(LPVOID* a)
	{ /* FIXME??? */ FNTRACE; return refcounted_wrapper<T>::m_real->GetDDInterface(a); }

    virtual HRESULT WINAPI PageLock(DWORD a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->PageLock(a); }

    virtual HRESULT WINAPI PageUnlock(DWORD a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->PageUnlock(a); }

    virtual HRESULT WINAPI SetSurfaceDesc(LPDDSURFACEDESC2 a, DWORD b)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->SetSurfaceDesc(a, b); }

    virtual HRESULT WINAPI SetPrivateData (REFGUID a, LPVOID b, DWORD c, DWORD d)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->SetPrivateData(a, b, c, d); }

	virtual HRESULT WINAPI GetPrivateData (REFGUID a, LPVOID b, LPDWORD c)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetPrivateData(a, b, c); }

    virtual HRESULT WINAPI FreePrivateData(REFGUID a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->FreePrivateData(a); }

    virtual HRESULT WINAPI GetUniquenessValue(LPDWORD a)
	{ FNTRACE; return refcounted_wrapper<T>::m_real->GetUniquenessValue(a); }

    virtual HRESULT WINAPI ChangeUniquenessValue()
	{ FNTRACE; return refcounted_wrapper<T>::m_real->ChangeUniquenessValue(); }
};

/*! concrete IDirectDrawSurface implementation */
struct fake_ddraw_surf : public fake_ddraw_surf_base<IDirectDrawSurface, fake_ddraw_surf> {
	explicit fake_ddraw_surf(IDirectDrawSurface* real)
		: fake_ddraw_surf_base<IDirectDrawSurface, fake_ddraw_surf>(real) {}
};

/*! concrete IDirectDrawSurface4 implementation */
struct fake_ddraw_surf4 : public fake_ddraw4_surf_base<IDirectDrawSurface4, fake_ddraw_surf4> {
	explicit fake_ddraw_surf4(IDirectDrawSurface4* real)
		: fake_ddraw4_surf_base<IDirectDrawSurface4, fake_ddraw_surf4>(real) {}
};

/*! concrete IDirectDrawSurface7 implementation */
struct fake_ddraw_surf7 : public fake_ddraw4_surf_base<IDirectDrawSurface7, fake_ddraw_surf7> {
	explicit fake_ddraw_surf7(IDirectDrawSurface7* real)
		: fake_ddraw4_surf_base<IDirectDrawSurface7, fake_ddraw_surf7>(real) {}

    virtual HRESULT WINAPI SetPriority(DWORD a)
	{ FNTRACE; return m_real->SetPriority(a); }

    virtual HRESULT WINAPI GetPriority(LPDWORD a)
	{ FNTRACE; return m_real->GetPriority(a); }

    virtual HRESULT WINAPI SetLOD(DWORD a)
	{ FNTRACE; return m_real->SetLOD(a); }

    virtual HRESULT WINAPI GetLOD(LPDWORD a)
	{ FNTRACE; return m_real->GetLOD(a); }
};

#endif
