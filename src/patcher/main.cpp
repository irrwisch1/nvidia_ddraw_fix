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

#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include "strncasecmp.h"

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	static const char appname[] = "nVidia DirectDraw Fix";
	char file[MAX_PATH] = { 0 };

	char pwd[MAX_PATH];
	GetCurrentDirectoryA( MAX_PATH-1, pwd );

	OPENFILENAMEA fn;
	memset( &fn, 0, sizeof(OPENFILENAMEA) );
	fn.lStructSize = sizeof(OPENFILENAMEA);
	fn.hInstance   = hinst;
	fn.lpstrFilter = "executable files\0*.exe\0\0";
	fn.nMaxFile    = MAX_PATH-1;
	fn.lpstrFile   = file;
	fn.lpstrTitle  = appname;
	fn.Flags       = OFN_HIDEREADONLY;
	fn.lpstrInitialDir = pwd;

	if ( GetOpenFileNameA(&fn) != 0 ) {
		std::vector<char> buf;
		{
			std::ifstream stream(file, std::ios::binary);

			stream.seekg( 0, std::ios::end );
			std::streampos size = stream.tellg();
			stream.seekg( 0, std::ios::beg );
			buf.resize(size);
			stream.read(&buf.front(), buf.size());

			if (!stream.good() ) {
				MessageBoxA(NULL, "Can't open file", appname, MB_ICONERROR | MB_OK);
				return EXIT_FAILURE;
			}
		}
		
		static const unsigned needle_len = 9;
		static const char needle[]  = "ddraw.dll";
		static const char needle2[] = "nvddf.dll";

		char* ptr = &buf.front();

		bool patched = false;
		bool found = false;
		for ( size_t i = 0; i < buf.size() - needle_len; ++i ) {
			if ( strncasecmp( ptr + i, needle, needle_len ) == 0 ) {
				memcpy( ptr + i, needle2, needle_len );
				patched = true;
				found = true;
				break;
			}

			if ( strncasecmp( ptr + i, needle2, needle_len ) == 0 ) {
				if ( MessageBoxA(NULL, "This .exe seems to be already patched. Do you want to remove the patch?",
					appname, MB_YESNO | MB_ICONQUESTION ) == IDYES ) {
					memcpy( ptr + i, needle, needle_len );
					found = true;
					break;
				}
				return EXIT_FAILURE;
			}
		}

		if ( !found ) {
			MessageBoxA(NULL, "This .exe doesn't appear to use direct draw, not patched", appname, MB_OK | MB_ICONINFORMATION);
			return EXIT_FAILURE;
		}

		if (patched)
			CopyFileA( file, (std::string(file) + ".nvddf_backup").c_str(), TRUE );

		#ifdef _MSC_VER
		_unlink(file);
		#else
		unlink(file);
		#endif

		std::ofstream stream(file, std::ios::binary);
		stream.write( ptr, buf.size() );

		if (!stream.good()) {
			MessageBoxA(NULL, "Can't write file. This can happen if another programm has this file still opened.",
				appname, MB_ICONERROR | MB_OK);
			return EXIT_FAILURE;
		}

		if (patched)
			MessageBoxA(NULL, "file successfully patched", appname, MB_OK | MB_ICONINFORMATION);

		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}
