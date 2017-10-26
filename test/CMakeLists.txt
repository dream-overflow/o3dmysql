#----------------------------------------------------------
# targets
#----------------------------------------------------------

#file(GLOB_RECURSE TARGET_SRC *.cpp .)
file(GLOB TARGET_SRC *.cpp .)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set(TARGET_NAME testmysql1-dbg)
elseif (${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
	set(TARGET_NAME testmysql1-odbg)
elseif (${CMAKE_BUILD_TYPE} MATCHES "Release")
	set(TARGET_NAME testmysql1)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_executable(${TARGET_NAME} ${TARGET_SRC})
target_link_libraries(${TARGET_NAME} o3dmysql${LIB_EXT} mysqlclient objective3d${LIB_EXT} ${OPENGL_gl_LIBRARY})
