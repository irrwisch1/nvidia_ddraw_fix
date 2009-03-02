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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include <ddraw.h>
#include <assert.h>
#include "trace.h"
#include "fake_ddraw.h"

HWND g_hwnd = NULL;

typedef HRESULT (WINAPI *pDirectDrawCreate)(GUID* a, LPDIRECTDRAW* b, IUnknown* c);

//! pointer to the real DirectDrawCreate function
static pDirectDrawCreate RealDirectDrawCreate = NULL;

//! handle to real ddraw dll
static HMODULE ddraw_handle;

//! instance of the fake ddraw interface
static fake_ddraw* ddraw = NULL;

//! load real direct draw dll
static void load_ddraw_dll()
{
	ddraw_handle = LoadLibraryA("ddraw.dll");
	RealDirectDrawCreate = (pDirectDrawCreate)GetProcAddress(ddraw_handle, "DirectDrawCreate");
}

#ifdef _DEBUG
//! allocate a console window for debugging purposes
static void alloc_console()
{
	AllocConsole();
	#if defined _MSC_VER && (_MSC_VER >= 1400)
	FILE *dummy;
	freopen_s(&dummy, "CONIN$", "rb",stdin);
	freopen_s(&dummy, "CONOUT$", "wb",stdout);
	freopen_s(&dummy, "CONOUT$", "wb",stderr);
	#else
	freopen("CONIN$", "rb", stdin);
	freopen("CONOUT$", "wb", stdout);
	freopen("CONOUT$", "wb", stderr);
	#endif
}

// little wrapper for FreeConsole that's put into atexit
static void free_console()
{
	using namespace std;

	// give me a chance to read what's printed on the console
	// before closing it
	std::cout << "\n\nPress a key to close" << std::endl;

	// from MSDN:
	// http://support.microsoft.com/?scid=kb%3Ben-us%3B43993&x=5&y=11
	while( _kbhit() ) _getch();

	_getch();

	FreeConsole();
}
#endif

extern "C" BOOL WINAPI DllMain(HANDLE /*handle*/, DWORD reason, LPVOID  /*reserved */)
{
	switch ( reason ) {
		case DLL_PROCESS_ATTACH:
			load_ddraw_dll();
			#ifdef _DEBUG
			alloc_console();
			#endif
			break;
		case DLL_PROCESS_DETACH:
			if ( ddraw )
				ddraw->Release();
			#ifdef _DEBUG
			free_console();
			#endif
			FreeLibrary(ddraw_handle);
			break;
	}

	return TRUE;
}

/*! our fake DirectDrawCreate function. This functions creates a fake direct draw
    instance and a real direct draw instance, and passes the real ddraw instance
	to the fake ddraw instance so that it can forward any calls to it
*/
extern "C" __declspec(dllexport) HRESULT WINAPI
#if defined(_MSC_VER)
FakeDirectDrawCreate
#else
DirectDrawCreate
#endif
(GUID* a, LPDIRECTDRAW* b, IUnknown* c)
{
	FNTRACE

	if(ddraw) {
		*b = static_cast<IDirectDraw*>(ddraw);
		ddraw->AddRef();
		return 0;
	}

	HRESULT hr = RealDirectDrawCreate(a, b, c);
	if(FAILED(hr))
		return hr;

	ddraw = new fake_ddraw(*b);

	*b = static_cast<IDirectDraw*>(ddraw);
	ddraw->AddRef();
	return 0;

	//FNTRACE
	//return DDrawCreate(a, b, c);
}

void show_error_box(const char* message, uint32_t flags)
{
	if ( g_hwnd != NULL )
		ShowWindow( g_hwnd, SW_MINIMIZE );
	MessageBoxA( NULL, message, "nVidia DirectDraw fix", flags);
	exit(EXIT_FAILURE);
}

//HRESULT WINAPI __declspec(dllexport) DirectDrawCreate(GUID* a, LPDIRECTDRAW* b, IUnknown* c) __attribute__ ((alias ("FakeDirectDrawCreate")));
