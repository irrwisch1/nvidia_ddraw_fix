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

#if defined(__GNUC__)
	#include <cxxabi.h>
#endif

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
}

void show_error_box(const char* message, uint32_t flags)
{
	if ( g_hwnd != NULL )
		ShowWindow( g_hwnd, SW_MINIMIZE );
	MessageBoxA( NULL, message, "nVidia DirectDraw fix", flags);
	exit(EXIT_FAILURE);
}

std::string demangle(const char* name)
{
	#if defined(__GNUC__)
	size_t length;
	int status;

	// try to demangle the mangled function name. If this fails
	// (status != 0) then we output the managled name (some names like
	// C functions are not mangled at all)
	char* demangled = abi::__cxa_demangle(name, NULL, &length, &status);

	if (status != 0)
		return name;
	else {
		std::string ret(demangled);
		free(demangled);
		return ret;
	}
	#else // MSVC doesn't mangle
	return name;
	#endif
}

#pragma pack(push)
#pragma pack(1)
typedef struct {
  unsigned short signature;
  unsigned int size;
  unsigned int reserved;
  unsigned int offset;
} bmp_header_t;
#pragma pack(pop)

typedef struct {
  unsigned int size;
  int width;
  int height;
  unsigned short planes;
  unsigned short bitcount;
  unsigned int compression;
  unsigned int sizeimage;
  int xpelspermeter;
  int ypelspermeter;
  unsigned int clrused;
  unsigned int clrimportant;
} bmp_info_t;


void write_bitmap(const char* filename, const unsigned char *m_data, unsigned m_width, unsigned m_height, unsigned m_bpp, unsigned src_stride)
{
  FILE *fp;
  bmp_header_t header;
  bmp_info_t info;

  //int pitch = (m_width + 3) & ~3;  
  int stride = (m_width * m_bpp + 3) & ~3;  

  int tablesize = 0;
  int imagesize = stride * m_height;

 // if (m_bpp <= 1)
 //   tablesize = sizeof(BMP8BitPalette::PaletteEntry) * (2<<(m_bpp*8-1));

  info.size          = sizeof(bmp_info_t);
  info.height        = -(int)m_height;
  info.width         = m_width;
  info.planes        = 1;
  info.bitcount      = (short) m_bpp * 8;
  info.compression   = 0;
  info.sizeimage     = imagesize;
  info.xpelspermeter = 0;
  info.ypelspermeter = 0;
  info.clrimportant  = 0; 
  info.clrused       = 0;

  header.signature   = 19778;
  header.reserved    = 0;
  header.offset      = sizeof(bmp_header_t) + sizeof(bmp_info_t) + tablesize;
  header.size        = imagesize + header.offset;
   
  fp = fopen(filename, "wb");
  fwrite(&header, sizeof(bmp_header_t), 1, fp);
  fwrite(&info, sizeof(bmp_info_t), 1, fp);

  //if (m_bpp <= 1)
  //  fwrite(m_palette.m_colors, tablesize, 1, fp);

  unsigned rw = m_width * m_bpp;
  for (unsigned i = 0; i < m_height; ++i ) {
	fwrite(m_data + i * src_stride, rw, 1, fp);

	for (unsigned j = 0; j < stride - rw; ++j)
		fputc(0, fp);
	/*
	int diff = src_stride - stride;
	if ( diff > 0 ) {
		for (unsigned j = 0; i < diff; ++i ) {
			fputc(0,fp);
		}
	}
	*/
  }

  fclose(fp);
}
