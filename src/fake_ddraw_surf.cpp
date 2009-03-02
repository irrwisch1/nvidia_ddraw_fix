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

#include "fake_ddraw_surf.h"
#include "fake_ddraw.h"
#include "trace.h"
#include "compat.h"
#include "main.h"
#include <assert.h>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

template<typename T>
T& operator<<(T& stream, const DDCOLORKEY& val)
{
	stream << '(' << val.dwColorSpaceHighValue << '/' << val.dwColorSpaceLowValue << ')';
	return stream;
}

template<typename T>
T& operator<<(T& stream, const RECT& val)
{
	stream << "(l:" << val.left << ",t:" << val.top << ",r:" << val.right << ",b:" << val.bottom << ')';
	return stream;
}

fake_ddraw_surf::fake_ddraw_surf(IDirectDrawSurface* real)
: m_refs(1), m_real(real)
{
}

HRESULT WINAPI fake_ddraw_surf::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	FNTRACE
	return m_real->QueryInterface(riid, ppvObj);
}

ULONG WINAPI fake_ddraw_surf::AddRef()
{
	FNTRACE
	return ++m_refs;
}

ULONG WINAPI fake_ddraw_surf::Release()
{
	FNTRACE
	if ( --m_refs == 0 ) {
		m_real->Release();
		outstream << "deleting fake_ddraw_surface" << std::endl;
		delete this;
		return 0;
	}

	return m_refs;
}

HRESULT WINAPI fake_ddraw_surf::AddAttachedSurface(LPDIRECTDRAWSURFACE a)
{
	FNTRACE
	if ( fake_ddraw_surf* surf = dynamic_cast<fake_ddraw_surf*>(a) )
		return m_real->AddAttachedSurface(surf->m_real);
	else
		return m_real->AddAttachedSurface(a);
}

HRESULT WINAPI fake_ddraw_surf::AddOverlayDirtyRect(LPRECT a)
{
	FNTRACE
	return m_real->AddOverlayDirtyRect(a);
}

template<typename T>
void fill(T* ptr, const RECT& rect, T val, unsigned pitch)
{
	const unsigned p = pitch / sizeof(T);

	for (int i = rect.top; i < rect.bottom; ++i)
		std::fill( &ptr[i * p + rect.left], &ptr[i * p + rect.right], val );
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

//#define NVIDIA_DDRAW_FIX_PROFILE

HRESULT WINAPI fake_ddraw_surf::Blt(LPRECT dest_rect, LPDIRECTDRAWSURFACE src_surf, LPRECT src_rect, DWORD flags, LPDDBLTFX blt_fx)
{
	FNTRACE

	// TODO wieder weg
	// Note: the bug doesn't seem to happen with blits on the full surface (dest_rect == NULL)
	if ( dest_rect && flags & DDBLT_COLORFILL ) {

		DDSURFACEDESC surf_desc;
		surf_desc.dwSize = sizeof(DDSURFACEDESC);

		// Note: the game freezes if I try to only lock the dest_rect region
		// I don't know why this happens. Locking the whole surface doesn't freeze
		// the game.
		HRESULT res = m_real->Lock(NULL, &surf_desc, DDLOCK_WRITEONLY, NULL);

		if ( FAILED(res) ) {
			LOG_STDERR("failed to lock surface");
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

			m_real->Unlock(NULL);
			return DD_OK;
		}
	}

	if ( fake_ddraw_surf* surf = dynamic_cast<fake_ddraw_surf*>(src_surf) )
		return m_real->Blt(dest_rect, surf->m_real, src_rect, flags, blt_fx);
	else
		return m_real->Blt(dest_rect, src_surf, src_rect, flags, blt_fx);
}

HRESULT WINAPI fake_ddraw_surf::BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
{
	FNTRACE
	return m_real->BltBatch(a, b, c);
}

HRESULT WINAPI fake_ddraw_surf::BltFast(DWORD x, DWORD y, LPDIRECTDRAWSURFACE src_surf, LPRECT src_rect, DWORD trans)
{
	FNTRACE

	if ( fake_ddraw_surf* surf = dynamic_cast<fake_ddraw_surf*>(src_surf) )
		return m_real->BltFast(x, y, surf->m_real, src_rect, trans);
	else
		return m_real->BltFast(x, y, src_surf, src_rect, trans);
}

HRESULT WINAPI fake_ddraw_surf::DeleteAttachedSurface(DWORD a, LPDIRECTDRAWSURFACE b)
{
	FNTRACE

	if ( fake_ddraw_surf* surf = dynamic_cast<fake_ddraw_surf*>(b) )
		return m_real->DeleteAttachedSurface(a, surf->m_real);
	else
		return m_real->DeleteAttachedSurface(a, b);
}

HRESULT WINAPI fake_ddraw_surf::EnumAttachedSurfaces(LPVOID a, LPDDENUMSURFACESCALLBACK b)
{
	FNTRACE
	return m_real->EnumAttachedSurfaces(a, b);
}

HRESULT WINAPI fake_ddraw_surf::EnumOverlayZOrders(DWORD a, LPVOID b, LPDDENUMSURFACESCALLBACK c)
{
	FNTRACE
	return m_real->EnumOverlayZOrders(a, b, c);
}

HRESULT WINAPI fake_ddraw_surf::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
	FNTRACE
	if ( fake_ddraw_surf* surf = dynamic_cast<fake_ddraw_surf*>(a) )
		return m_real->Flip(surf->m_real, b);
	else
		return m_real->Flip(a, b);
}

HRESULT WINAPI fake_ddraw_surf::GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE * b)
{
	FNTRACE
	HRESULT res = m_real->GetAttachedSurface(a, b);
	*b = new fake_ddraw_surf(*b);
	return res;
}

HRESULT WINAPI fake_ddraw_surf::GetBltStatus(DWORD a)
{
	FNTRACE
	return m_real->GetBltStatus(a);
}

HRESULT WINAPI fake_ddraw_surf::GetCaps(LPDDSCAPS a)
{
	FNTRACE
	return m_real->GetCaps(a);
}

HRESULT WINAPI fake_ddraw_surf::GetClipper(LPDIRECTDRAWCLIPPER* a)
{
	FNTRACE
	return m_real->GetClipper(a);
}

HRESULT WINAPI fake_ddraw_surf::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
	FNTRACE
	return m_real->GetColorKey(a, b);
}

HRESULT WINAPI fake_ddraw_surf::GetDC(HDC* a)
{
	FNTRACE
	return m_real->GetDC(a);
}

HRESULT WINAPI fake_ddraw_surf::GetFlipStatus(DWORD a)
{
	FNTRACE
	return m_real->GetFlipStatus(a);
}

HRESULT WINAPI fake_ddraw_surf::GetOverlayPosition(LPLONG a, LPLONG b)
{
	FNTRACE
	return m_real->GetOverlayPosition(a, b);
}

HRESULT WINAPI fake_ddraw_surf::GetPalette(LPDIRECTDRAWPALETTE* a)
{
	FNTRACE
	return m_real->GetPalette(a);
}

HRESULT WINAPI fake_ddraw_surf::GetPixelFormat(LPDDPIXELFORMAT a)
{
	FNTRACE
	return m_real->GetPixelFormat(a);
}

HRESULT WINAPI fake_ddraw_surf::GetSurfaceDesc(LPDDSURFACEDESC a)
{
	FNTRACE
	return m_real->GetSurfaceDesc(a);
}

HRESULT WINAPI fake_ddraw_surf::Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC b)
{
	FNTRACE
	return m_real->Initialize(a, b);
}

HRESULT WINAPI fake_ddraw_surf::IsLost()
{
	FNTRACE
	return m_real->IsLost();
}

HRESULT WINAPI fake_ddraw_surf::Lock(LPRECT a,LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
	FNTRACE
	return m_real->Lock(a, b, c, d);
}

HRESULT WINAPI fake_ddraw_surf::ReleaseDC(HDC a)
{
	FNTRACE
	return m_real->ReleaseDC(a);
}

HRESULT WINAPI fake_ddraw_surf::Restore()
{
	FNTRACE
	return m_real->Restore();
}

HRESULT WINAPI fake_ddraw_surf::SetClipper(LPDIRECTDRAWCLIPPER a)
{
	FNTRACE
	return m_real->SetClipper(a);
}

HRESULT WINAPI fake_ddraw_surf::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
	FNTRACE
	outstream << "SetColorKey: " << *b << std::endl;
	return m_real->SetColorKey(a, b);
}

HRESULT WINAPI fake_ddraw_surf::SetOverlayPosition(LONG a, LONG b)
{
	FNTRACE
	return m_real->SetOverlayPosition(a, b);
}

HRESULT WINAPI fake_ddraw_surf::SetPalette(LPDIRECTDRAWPALETTE a)
{
	FNTRACE
	return m_real->SetPalette(a);
}

HRESULT WINAPI fake_ddraw_surf::Unlock(LPVOID a)
{
	FNTRACE
	return m_real->Unlock(a);
}

HRESULT WINAPI fake_ddraw_surf::UpdateOverlay(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDOVERLAYFX e)
{
	FNTRACE
	if ( fake_ddraw_surf* surf = dynamic_cast<fake_ddraw_surf*>(b) )
		return m_real->UpdateOverlay(a, surf->m_real, c, d, e);
	else
		return m_real->UpdateOverlay(a, b, c, d, e);
}

HRESULT WINAPI fake_ddraw_surf::UpdateOverlayDisplay(DWORD a)
{
	FNTRACE
	return m_real->UpdateOverlayDisplay(a);
}

HRESULT WINAPI fake_ddraw_surf::UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE b)
{
	FNTRACE
	if ( fake_ddraw_surf* surf = dynamic_cast<fake_ddraw_surf*>(b) )
		return m_real->UpdateOverlayZOrder(a, surf->m_real);
	else
		return m_real->UpdateOverlayZOrder(a, b);
}
