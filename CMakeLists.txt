cmake_minimum_required(VERSION 3.15)
project(WoWTool)

set(CMAKE_CXX_STANDARD 20)
include_directories(src)
include_directories(src/res)

# Modified from https://stackoverflow.com/a/1380048
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
	IF(MSVC)
		GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
		GET_FILENAME_COMPONENT(PrecompiledFilename ${PrecompiledHeader} NAME)
		SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
		SET(Sources ${${SourcesVar}})

		SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
									PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledFilename}\" /Fp\"${PrecompiledBinary}\""
												OBJECT_OUTPUTS "${PrecompiledBinary}")
		SET_SOURCE_FILES_PROPERTIES(${Sources}
									PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
												OBJECT_DEPENDS "${PrecompiledBinary}")  
		# Add precompiled header to SourcesVar
		LIST(APPEND ${SourcesVar} ${PrecompiledSource})
	ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

set(wow_files
	src/WoW/Defines.h
	src/WoW/Defines_8606.h
	src/WoW/Defines_12340.h)

set(wow_classes_files
	src/WoW/Classes/Camera.cpp
	src/WoW/Classes/Camera.h
	src/WoW/Classes/Engine.cpp
	src/WoW/Classes/Engine.h
	src/WoW/Classes/Player.cpp
	src/WoW/Classes/Player.h
	src/WoW/Classes/WoWManager.cpp
	src/WoW/Classes/WoWManager.h)

set(res_files
	src/res/resource.h
	src/res/WoWTool.rc)

set(windows_files
	src/MainWindow.cpp
	src/MainWindow.h
	src/main.cpp
	src/main.h)

set(misc_files
	src/version.txt)

if(MSVC)
	add_definitions(-DUNICODE -D_UNICODE) # Make sure we're using the Unicode WinAPI
	ADD_MSVC_PRECOMPILED_HEADER("src/preCompiled.h" "src/preCompiled.cpp" source_files)
	source_group("WoW" FILES ${wow_files})
	source_group("WoW\\Classes" FILES ${wow_classes_files})
	source_group("Resources" FILES ${res_files})
	source_group("Windows" FILES ${windows_files})
	source_group("" FILES ${misc_files})
endif()

set(all_source_files
	${wow_files}
	${wow_classes_files}
	${res_files}
	${windows_files}
	${misc_files})

add_executable(WoWTool WIN32 ${all_source_files})

install(TARGETS WoWTool RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
