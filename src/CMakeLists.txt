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

FILE(GLOB HEADERS *.h)
FILE(GLOB SOURCES *.cpp)
 
# TODO: replace with proper FindDirectX.cmake or something
include_directories ( ${include_directories}
    	"$ENV{DXSDK_DIR}/Include" )

link_directories ( "$ENV{DXSDK_DIR}/Lib/x86" )

# the rc file must be added to the executable. It can't be added to static libs
SET(win32_resource_obj "")
SET(win32_resource_src "")

# cmake doesn't support rc files on mingw:
# http://www.cmake.org/Bug/view.php?id=4068
#if(WIN32)
#	if(MINGW)
#		# resource compilation for mingw
#		ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version_rc.o
#			COMMAND ${WINDRES} -I${nvidia_ddraw_fix_SOURCE_DIR}
#			-i${nvidia_ddraw_fix_SOURCE_DIR}/src/version.rc
#			-o ${CMAKE_CURRENT_BINARY_DIR}/version_rc.o)
#		SET(win32_resource_obj ${CMAKE_CURRENT_BINARY_DIR}/version_rc.o)
#	else(MINGW)
#		SET(win32_resource_src ${nvidia_ddraw_fix_SOURCE_DIR}/src/version.rc)
#	endif(MINGW)
#endif(WIN32)

add_rc_file (version_info version.rc )
message(STATUS ${win32_resource_obj})

add_library( nvddf SHARED 
	${version_info}
	#	${win32_resource_obj}
	#${win32_resource_src}
	${SOURCES}
	${HEADERS}
	exports.def
)

target_link_libraries ( nvddf dxguid )

add_subdirectory ( patcher )

