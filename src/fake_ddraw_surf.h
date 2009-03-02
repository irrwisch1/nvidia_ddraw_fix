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

class fake_ddraw_surf : public IDirectDrawSurface {
	public:
		explicit fake_ddraw_surf(IDirectDrawSurface* real);
		HRESULT WINAPI QueryInterface(REFIID riid, LPVOID * ppvObj);
		ULONG WINAPI AddRef();
		ULONG WINAPI Release();
		HRESULT WINAPI AddAttachedSurface(LPDIRECTDRAWSURFACE a);
		HRESULT WINAPI AddOverlayDirtyRect(LPRECT a);
		HRESULT WINAPI Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e);
		HRESULT WINAPI BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c);
		HRESULT WINAPI BltFast(DWORD a, DWORD b, LPDIRECTDRAWSURFACE c, LPRECT d, DWORD e);
		HRESULT WINAPI DeleteAttachedSurface(DWORD a, LPDIRECTDRAWSURFACE b);
		HRESULT WINAPI EnumAttachedSurfaces(LPVOID a, LPDDENUMSURFACESCALLBACK b);
		HRESULT WINAPI EnumOverlayZOrders(DWORD a, LPVOID b, LPDDENUMSURFACESCALLBACK c);
		HRESULT WINAPI Flip(LPDIRECTDRAWSURFACE a, DWORD b);
		HRESULT WINAPI GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE * b);
		HRESULT WINAPI GetBltStatus(DWORD a);
		HRESULT WINAPI GetCaps(LPDDSCAPS a);
		HRESULT WINAPI GetClipper(LPDIRECTDRAWCLIPPER* a);
		HRESULT WINAPI GetColorKey(DWORD a, LPDDCOLORKEY b);
		HRESULT WINAPI GetDC(HDC* a);
		HRESULT WINAPI GetFlipStatus(DWORD a);
		HRESULT WINAPI GetOverlayPosition(LPLONG a, LPLONG b);
		HRESULT WINAPI GetPalette(LPDIRECTDRAWPALETTE* a);
		HRESULT WINAPI GetPixelFormat(LPDDPIXELFORMAT a);
		HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a);
		HRESULT WINAPI Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC b);
		HRESULT WINAPI IsLost();
		HRESULT WINAPI Lock(LPRECT a,LPDDSURFACEDESC b, DWORD c, HANDLE d);
		HRESULT WINAPI ReleaseDC(HDC a);
		HRESULT WINAPI Restore();
		HRESULT WINAPI SetClipper(LPDIRECTDRAWCLIPPER a);
		HRESULT WINAPI SetColorKey(DWORD a, LPDDCOLORKEY b);
		HRESULT WINAPI SetOverlayPosition(LONG a, LONG b);
		HRESULT WINAPI SetPalette(LPDIRECTDRAWPALETTE a);
		HRESULT WINAPI Unlock(LPVOID a);
		HRESULT WINAPI UpdateOverlay(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDOVERLAYFX e);
		HRESULT WINAPI UpdateOverlayDisplay(DWORD a);
		HRESULT WINAPI UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE b);

	private:
		unsigned m_refs;
		IDirectDrawSurface *m_real;
};

#endif
