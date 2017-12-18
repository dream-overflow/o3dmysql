cmake_minimum_required (VERSION 2.8)
project (O3D_MYSQL)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
   message(STATUS "Setting build type to 'Debug' as none was specified.")
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
   # Set the possible values of build type for cmake-gui
   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
     "MinSizeRel" "RelWithDebInfo")
endif()

if(NOT MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Zi /D_DEBUG /EHsc /MP /MDd /DO3D_DISABLE_RAM_MEMORY_MANAGER /DO3D_DISABLE_FAST_MEMORY_MANAGER")
	set(CMAKE_CXX_FLAGS_RELEASE        "/O2 /DNDEBUG /EHsc /MP /MD /DO3D_DISABLE_RAM_MEMORY_MANAGER /DO3D_DISABLE_FAST_MEMORY_MANAGER /DO3D_DISABLE_GPU_MEMORY_MANAGER /DO3D_DISABLE_SPU_MEMORY_MANAGER")
	set(CMAKE_CXX_FLAGS_DEBUG          "/D_DEBUG /Zi /EHsc /MP /MDd /DO3D_DISABLE_RAM_MEMORY_MANAGER /DO3D_DISABLE_FAST_MEMORY_MANAGER")
	set(CMAKE_CXX_FLAGS_DEBUGFULL      "/D_DEBUG /Zi /EHsc /MP /MDd")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO   "/O2 /Zi /D_DEBUG /EHsc /MP /MD /DO3D_DISABLE_RAM_MEMORY_MANAGER /DO3D_DISABLE_FAST_MEMORY_MANAGER")
	set(CMAKE_C_FLAGS_RELEASE          "/O2 /DNDEBUG /EHsc /MP /MD /DO3D_DISABLE_RAM_MEMORY_MANAGER /DO3D_DISABLE_FAST_MEMORY_MANAGER /DO3D_DISABLE_GPU_MEMORY_MANAGER /DO3D_DISABLE_SPU_MEMORY_MANAGER")
	set(CMAKE_C_FLAGS_DEBUG            "/D_DEBUG /Zi /EHsc /MP /MDd /DO3D_DISABLE_RAM_MEMORY_MANAGER /DO3D_DISABLE_FAST_MEMORY_MANAGER")
	set(CMAKE_C_FLAGS_DEBUGFULL        "/D_DEBUG /Zi /EHsc /MP /MDd")
else()
    # compiler options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fvisibility=hidden")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggbd -D_DEBUG -DO3D_DISABLE_RAM_MEMORY_MANAGER -DO3D_DISABLE_FAST_MEMORY_MANAGER")
	set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG -DO3D_DISABLE_RAM_MEMORY_MANAGER -DO3D_DISABLE_FAST_MEMORY_MANAGER -DO3D_DISABLE_GPU_MEMORY_MANAGER -DO3D_DISABLE_SPU_MEMORY_MANAGER")
	set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -ggdb -D_DEBUG -DO3D_DISABLE_RAM_MEMORY_MANAGER -DO3D_DISABLE_FAST_MEMORY_MANAGER")
	set(CMAKE_CXX_FLAGS_DEBUGFULL      "-O0 -ggdb -D_DEBUG")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -ggdb -D_DEBUG -DO3D_DISABLE_RAM_MEMORY_MANAGER -DO3D_DISABLE_FAST_MEMORY_MANAGER")
	set(CMAKE_C_FLAGS_RELEASE          "-O2 -DNDEBUG -DO3D_DISABLE_RAM_MEMORY_MANAGER -DO3D_DISABLE_FAST_MEMORY_MANAGER -DO3D_DISABLE_GPU_MEMORY_MANAGER -DO3D_DISABLE_SPU_MEMORY_MANAGER")
	set(CMAKE_C_FLAGS_DEBUG            "-O0 -ggdb -D_DEBUG -DO3D_DISABLE_RAM_MEMORY_MANAGER -DO3D_DISABLE_FAST_MEMORY_MANAGER")
	set(CMAKE_C_FLAGS_DEBUGFULL        "-O0 -ggbd -D_DEBUG")
endif()

#----------------------------------------------------------
# externals libraries
#----------------------------------------------------------

include(FindOpenGL)

# Objective3dConfig.h
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	if(MINGW)
		set(LIB_PATH $ENV{CROSSROAD_PREFIX})
	else()
		set(LIB_PATH $ENV{LIB_PATH})
	endif()
else()
	set(LIB_PATH $ENV{PREFIX})
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	include_directories(${LIB_PATH}/lib/objective3d-dbg)
	set(LIB_EXT "-dbg")
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
	include_directories(${LIB_PATH}/lib/objective3d-odbg)
	set(LIB_EXT "-odbg")
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
	include_directories(${LIB_PATH}/lib/objective3d)
	set(LIB_EXT "")
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	include_directories(${LIB_PATH}/lib/objective3d-dbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
	include_directories(${LIB_PATH}/lib/objective3d-odbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
	include_directories(${LIB_PATH}/lib/objective3d)
endif()

# Windows MSVC
if(NOT MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set (O3D_PATH $ENV{O3D_PATH})

	set(O3D_INCLUDES "${O3D_PATH}/include")
	set(O3D_LIBRARIES_DIR "${O3D_PATH}/dist/lib/${CMAKE_BUILD_TYPE}")
	
	include_directories(${O3D_INCLUDES})
	include_directories(${O3D_LIBRARIES_DIR}/objective3d)
	link_directories(${O3D_LIBRARIES_DIR})
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BUILD_TYPE}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BUILD_TYPE}/bin)

include_directories("${PROJECT_SOURCE_DIR}/include")

#----------------------------------------------------------
# options
#----------------------------------------------------------

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter -std=c++14 -fexceptions -Wextra")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter -std=c++14 -fexceptions -Wextra")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	if(MINGW)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter -std=c++14 -fexceptions -Wextra -DO3D_IMPORT_DLL")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DO3D_IMPORT_DLL")
	endif()
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

#----------------------------------------------------------
# targets
#----------------------------------------------------------

add_subdirectory(src)
add_subdirectory(test)
