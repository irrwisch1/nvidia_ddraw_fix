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

#include "fake_ddraw.h"
#include "fake_ddraw_surf.h"
#include "trace.h"
#include "main.h"

/*********************
 * v1 implementation
 *********************/
HRESULT WINAPI fake_ddraw::QueryInterface(REFIID riid, LPVOID* obp)
{
	FNTRACE

	if ( riid == IID_IDirectDraw ) {
		LOG_STDERR("requesting ddraw 1")
		*obp = this;
		AddRef();
		return DD_OK;
	}
	else if ( riid == IID_IDirectDraw2 ) {
		LOG_STDERR("requesting ddraw 2")
		HRESULT hr = m_real->QueryInterface( IID_IDirectDraw2, obp );

		if ( *obp == NULL ) {
			LOG_STDERR("could not retrieve ddraw 2 interface");
			return hr;
		}
		
		*obp = new fake_ddraw2( static_cast<IDirectDraw2*>(*obp) );
		return DD_OK;
	}
	else if ( riid == IID_IDirectDraw4 ) 
		LOG_STDERR("requesting ddraw 4")
	else if ( riid == IID_IDirectDraw7 ) 
		LOG_STDERR("requesting ddraw 7")
}

/*********************
 * v2 implementation
 *********************/
HRESULT WINAPI fake_ddraw2::QueryInterface(REFIID riid, LPVOID* obp)
{
	FNTRACE

	if ( riid == IID_IDirectDraw ) 
		LOG_STDERR("requesting ddraw 1")
	else if ( riid == IID_IDirectDraw2 ) 
		LOG_STDERR("requesting ddraw 2")
	else if ( riid == IID_IDirectDraw4 ) 
		LOG_STDERR("requesting ddraw 4")
	else if ( riid == IID_IDirectDraw7 ) 
		LOG_STDERR("requesting ddraw 7")

	*obp = this;
	AddRef();
	return DD_OK;
}
