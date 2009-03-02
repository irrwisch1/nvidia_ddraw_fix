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

PROJECT( nvidia_ddraw_fix )
cmake_minimum_required(VERSION 2.6)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# override this with cmake -D CMAKE_BUILD_TYPE:STRING="Debug" CMakeLists.txt
IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Debug CACHE STRING "build type" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# add the build type (Release/Debug) to the output path
# this isn't required for MSVC as cmake does this automatically
IF(MSVC AND NOT CMAKE_BUILD_TOOL MATCHES "nmake")
	SET ( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin )
ELSE(MSVC AND NOT CMAKE_BUILD_TOOL MATCHES "nmake")
	SET ( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE} )
ENDIF(MSVC AND NOT CMAKE_BUILD_TOOL MATCHES "nmake")

# MinGW specific
IF(CMAKE_COMPILER_IS_MINGW)
	SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-auto-image-base,--enable-runtime-pseudo-reloc,--add-stdcall-alias,--output-def,${LIBRARY_OUTPUT_PATH}/out.def")
ENDIF(CMAKE_COMPILER_IS_MINGW)

# increase warning level
SET(CMAKE_CXX_WARNING_LEVEL 4)

#IF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
IF(MSVC)
   # Use the highest warning level for visual studio.
   SET(CMAKE_CXX_WARNING_LEVEL 4)
   IF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
     STRING(REGEX REPLACE "/W[0-4]" "/W4"
            CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
   ELSE(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
   ENDIF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
   #ENDIF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
ENDIF(MSVC)

INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES (stdint.h HAVE_STDINT_H)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory(src)

