# nVidia DirectDraw Fix
# Copyright (c) 2009 Julien Langer
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

macro(ADD_RC_FILE _var _file)
	#foreach(arg ${ARGV})
		if(MINGW)
			# resource compilation for mingw
			ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_file}.o
				COMMAND ${WINDRES}
				-i${_file}
				-o ${CMAKE_CURRENT_BINARY_DIR}/${_file}.o)
			SET(${_var} ${CMAKE_CURRENT_BINARY_DIR}/${_file}.o)
		else(MINGW)
			# the cmake msvc generator can deal with .rc files directly
			SET(${_var} ${_file})
		endif(MINGW)
	#endforeach(arg)
endmacro(ADD_RC_FILE)

