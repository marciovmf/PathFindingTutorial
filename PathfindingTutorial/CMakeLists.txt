
cmake_minimum_required(VERSION 2.6)

project(Pathfinder)

#Enable c++11 features
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp NavigationGrid.h NavigationNode.h Pathfinder.h types.h)
set(CMAKE_CONFIGURATION_TYPES Release)
set(CMAKE_BUILD_TYPE Release)

## Estamos compilando 32 ou 64 bit ?
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	set(64BIT 1)
	MESSAGE("Building for 64bit")
else()
	MESSAGE("Building for 32bit")
endif()

if (WIN32)

	add_definitions(-DWIN32 -subsystem:windows)
	set(SDL2_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/dep/windows/SDL2/include/)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "WINDOWS")
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "WINDOWS")
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
	
	# Windows 64bit
	if(64BIT)
		MESSAGE( "pointing to 64bit libs" )
		set(SDL2_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/dep/windows/SDL2/lib/x64/)
		set(SDL2_LIBRARY ${SDL2_LIBRARY_DIR}/SDL2.lib)		
	# Windows 32bit
	else	(64BIT)
		MESSAGE( "pointing to 32bit libs" )
		set(SDL2_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/dep/windows/SDL2/lib/x86/)
		set(SDL2_LIBRARY ${SDL2_LIBRARY_DIR}/SDL2.lib)
	endif(64BIT)	

	ADD_CUSTOM_COMMAND(
	   TARGET ${PROJECT_NAME}
	   POST_BUILD
	   COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_LIBRARY_DIR}/SDL2.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
	 )		

else()
	#include FINDSDL2 module
	set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
	find_package(SDL2 REQUIRED)
	
	set(CMAKE_CXX_FLAGS -std=c++11)

endif()
	 
	 ADD_CUSTOM_COMMAND(
	   TARGET ${PROJECT_NAME}
	   POST_BUILD
	   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/tile.bmp $<TARGET_FILE_DIR:${PROJECT_NAME}>	   
	 )	
	 


include_directories(${SDL2_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

 