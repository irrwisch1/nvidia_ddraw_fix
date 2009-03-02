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

#ifndef REFCOUNTED_WRAPPER_H
#define REFCOUNTED_WRAPPER_H

/*! implements the reference counting for the wrapped COM
    interfaces
*/
template<typename T>
struct refcounted_wrapper : public T {

	explicit refcounted_wrapper(T* real)
		: m_real(real), m_refs(1) {}

	virtual ~refcounted_wrapper() = 0 {}

	virtual ULONG WINAPI AddRef()	
	{
		return ++m_refs;
	}

	virtual ULONG WINAPI Release()
	{
		if ( --m_refs == 0 ) {
			m_real->Release();
			delete this;
			return 0;
		}

		return m_refs;
	}

	protected:
		T* m_real;

	private:
		ULONG m_refs;
};

#endif
