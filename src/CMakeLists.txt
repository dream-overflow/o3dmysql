#----------------------------------------------------------
# targets
#----------------------------------------------------------

#file(GLOB_RECURSE TARGET_SRC *.cpp .)
file(GLOB TARGET_SRC *.cpp .)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DO3D_MYSQL_EXPORT_DLL")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DO3D_MYSQL_EXPORT_DLL")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	if(MINGW)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DO3D_MYSQL_EXPORT_DLL")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DO3D_MYSQL_EXPORT_DLL")
	endif()
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set(TARGET_NAME o3dmysql-dbg)
elseif (${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
	set(TARGET_NAME o3dmysql-odbg)
elseif (${CMAKE_BUILD_TYPE} MATCHES "Release")
	set(TARGET_NAME o3dmysql)
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_library(${TARGET_NAME} SHARED ${TARGET_SRC})
target_link_libraries(${TARGET_NAME} mysqlclient ${OBJECTIVE3D_LIBRARY})

#----------------------------------------------------------
# install
#----------------------------------------------------------

file(GLOB TARGET_HXX ../include/o3d/mysql/*.h)

install (FILES ${TARGET_HXX} DESTINATION include/o3d/mysql)
install (TARGETS ${TARGET_NAME} DESTINATION lib)
