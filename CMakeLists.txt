cmake_minimum_required( VERSION 2.8 )

project( "ck-cmd" )
include(ExternalProject)

option(PROJECT_STATIC_RUNTIME "Build with static runtime libs (/MT)" ON)
option(PROJECT_GUI "Build GUI" ON)
UNSET(HAVE_SPEEDTREE CACHE)
option(HAVE_SPEEDTREE "Build with speedtree support" OFF)


if (HAVE_SPEEDTREE)
  MESSAGE(STATUS "HAVE_SPEEDTREE")
  add_definitions(-DHAVE_SPEEDTREE)
endif()

message( "[MAIN]: building ck-cmd..." )

#External Projects
ExternalProject_Add(googletest
                    PREFIX "external"
                    GIT_REPOSITORY https://github.com/google/googletest.git
					GIT_TAG master
                    INSTALL_COMMAND "")
ExternalProject_Get_Property(googletest SOURCE_DIR BINARY_DIR)
set (GTEST_INCLUDE_DIRS "${SOURCE_DIR}/googletest/include")
set (GTEST_LIBRARIES debug "${BINARY_DIR}/lib/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtestd${CMAKE_STATIC_LIBRARY_SUFFIX}"
					optimized "${BINARY_DIR}/lib/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")

set (TEST_SRC "${CMAKE_SOURCE_DIR}/test/main.cpp")

ExternalProject_Add(zlib
                    PREFIX "external"
                    URL "https://zlib.net/zlib-1.2.11.tar.gz"
                    INSTALL_COMMAND "")
ExternalProject_Get_Property(zlib SOURCE_DIR BINARY_DIR)
set (ZLIB_INCLUDE_DIRS ${SOURCE_DIR} ${BINARY_DIR})
message("ZLIB_INCLUDE_DIRS:" ${ZLIB_INCLUDE_DIRS})
IF (MSVC)
    set (ZLIB_LIBRARIES optimized "${BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}"
                    debug "${BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstaticd${CMAKE_STATIC_LIBRARY_SUFFIX}")
ELSE ()
    set (ZLIB_LIBRARIES "${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}")
ENDIF ()

ExternalProject_Add(docopt
                    PREFIX "external"
                    GIT_REPOSITORY "https://github.com/docopt/docopt.cpp.git"
                    GIT_TAG master
                    INSTALL_COMMAND ""
					CMAKE_ARGS -DCMAKE_CXX_FLAGS_DEBUG=/MTd -DCMAKE_CXX_FLAGS_RELEASE=/MT -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=/MT)
ExternalProject_Get_Property(docopt SOURCE_DIR BINARY_DIR)
set (DOCOPT_INCLUDE_DIRS ${SOURCE_DIR} ${BINARY_DIR})
message("DOCOPT_INCLUDE_DIRS:" ${DOCOPT_INCLUDE_DIRS})
IF (MSVC)
    set (DOCOPT_LIBRARIES optimized "${BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}docopt_s${CMAKE_STATIC_LIBRARY_SUFFIX}"
                    debug "${BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}docopt_s${CMAKE_STATIC_LIBRARY_SUFFIX}")
ELSE ()
    set (DOCOPT_LIBRARIES "${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}docopt${CMAKE_STATIC_LIBRARY_SUFFIX}")
ENDIF ()

include_directories("${CMAKE_SOURCE_DIR}/src"
                    "${CMAKE_SOURCE_DIR}/include"
                    ${GTEST_INCLUDE_DIRS})

# Libraries

# DirectXTex

set ( DIRECTXTEX_INCLUDES_PATH
	"${CMAKE_SOURCE_DIR}/lib/DirectXTex/include"
)

add_library(
	DirectXTex
	STATIC 
	IMPORTED GLOBAL
)

set_target_properties(
	DirectXTex 
	PROPERTIES IMPORTED_LOCATION
	"${CMAKE_SOURCE_DIR}/lib/DirectXTex/lib/Desktop_2017_Win10/Win32/Release/DirectXTex.lib"
)

set_target_properties(
	DirectXTex
	PROPERTIES IMPORTED_LOCATION_DEBUG             
	"${CMAKE_SOURCE_DIR}/lib/DirectXTex/lib/Desktop_2017_Win10/Win32/Debug/DirectXTex.lib"
)

# Loki

set ( LOKI_INCLUDES_PATH
	"${CMAKE_SOURCE_DIR}/lib/loki/include"
)

add_library(
	loki
	STATIC 
	IMPORTED GLOBAL
)

set_target_properties(
	loki 
	PROPERTIES IMPORTED_LOCATION             
	"${CMAKE_SOURCE_DIR}/lib/loki/release/loki.lib"
)

set_target_properties(
	loki
	PROPERTIES IMPORTED_LOCATION_DEBUG             
	"${CMAKE_SOURCE_DIR}/lib/loki/debug/loki.lib"
)

# CBash

set ( CBASH_INCLUDES_PATH
	"${CMAKE_SOURCE_DIR}/lib/cbash"
	"${CMAKE_SOURCE_DIR}/lib/cbash/include/cbash"
)

add_library(
	cbash
	STATIC 
	IMPORTED GLOBAL
)

set_target_properties(
	cbash 
	PROPERTIES IMPORTED_LOCATION             
	"${CMAKE_SOURCE_DIR}/lib/cbash/lib/x86/release/CBash.lib"
)

set_target_properties(
	cbash
	PROPERTIES IMPORTED_LOCATION_DEBUG             
	"${CMAKE_SOURCE_DIR}/lib/cbash/lib/x86/debug/CBash.lib"
)

if (HAVE_SPEEDTREE)
# speedtree 5.1.1
set ( SPDT_INCLUDES_PATH
	"${CMAKE_SOURCE_DIR}/vendor/speedtree5.1.1/Include"
)

set ( SPEEDTREE_LIBS
	SpeedTreeCore_v5.1_VC90MT_Static
	SpeedTreeForest_v5.1_VC90MT_Static
)

foreach(SPEEDTREE_LIB IN LISTS SPEEDTREE_LIBS)
	message( "[MAIN]: adding ${SPEEDTREE_LIB}" )
	
	add_library(
		${SPEEDTREE_LIB}
		STATIC 
		IMPORTED GLOBAL
    )

	set_target_properties(
		${SPEEDTREE_LIB} 
		PROPERTIES IMPORTED_LOCATION             
		"${CMAKE_SOURCE_DIR}/vendor/speedtree5.1.1/Lib/Windows/VC9/${SPEEDTREE_LIB}.lib"
    )

	set_target_properties(
		${SPEEDTREE_LIB} 
		PROPERTIES IMPORTED_LOCATION_DEBUG             
		"${CMAKE_SOURCE_DIR}/vendor/speedtree5.1.1/Lib/Windows/VC9/${SPEEDTREE_LIB}_d.lib"
    )

endforeach(SPEEDTREE_LIB)
endif(HAVE_SPEEDTREE)

# havok_2010_2_0

set ( HAVOK_INCLUDES_PATH
	"${CMAKE_SOURCE_DIR}/vendor/havok_2010_2_0/Source"
)

set ( HAVOK_LIBS
	hkaAnimation
	hkaInternal
	hkaRagdoll
	hkBase
	hkCompat
	hkgBridge
	hkgCommon
	hkgDx10
	hkgDx9
	hkgDx9s
	hkGeometryUtilities
	hkgOgl
	hkgOglES
	hkgOglES2
	hkgOgls
	hkInternal
	hkpCollide
	hkpConstraintSolver
	hkpDynamics
	hkpInternal
	hkpUtilities
	hkpVehicle
	hkSceneData
	hksCommon
	hkSerialize
	hksXAudio2
	hkVisualize
)

foreach(havok_lib IN LISTS HAVOK_LIBS)
	message( "[MAIN]: adding ${havok_lib}" )
	
	add_library(
		${havok_lib}
		STATIC 
		IMPORTED GLOBAL
    )

	set_target_properties(
		${havok_lib} 
		PROPERTIES IMPORTED_LOCATION             
		"${CMAKE_SOURCE_DIR}/vendor/havok_2010_2_0/Lib/win32_net_9-0/release_multithreaded/${havok_lib}.lib"
    )

	set_target_properties(
		${havok_lib} 
		PROPERTIES IMPORTED_LOCATION_DEBUG             
		"${CMAKE_SOURCE_DIR}/vendor/havok_2010_2_0/Lib/win32_net_9-0/debug_multithreaded/${havok_lib}.lib"
    )

endforeach(havok_lib)

#2010 Behavior compat

set(HAVOK_COMPAT_INCLUDES_PATH
	"${CMAKE_SOURCE_DIR}/lib/havok_2010_2_0/compat/"
)

file(GLOB HAVOK_COMPAT_HEADERS
	"${CMAKE_SOURCE_DIR}/lib/havok_2010_2_0/compat/*.h"
	"${CMAKE_SOURCE_DIR}/lib/havok_2010_2_0/compat/*.inl"
)

file(GLOB HAVOK_COMPAT_SRC
	"${CMAKE_SOURCE_DIR}/lib/havok_2010_2_0/compat/*.cpp"
)

#Bethesda Animation Data

set(BETH_ANIMDATA_PATH
	"${CMAKE_SOURCE_DIR}/include/bs"
)

file(GLOB BETH_ANIMDATA_HEADERS
	"${CMAKE_SOURCE_DIR}/include/bs/*.h"
)

#NIFlib

set(NIFLIB_INCLUDES_PATH
	"${CMAKE_SOURCE_DIR}/lib/nif/include"
)

file(GLOB NIF_CORE_HEADERS
	"${CMAKE_SOURCE_DIR}/lib/nif/include/*.h"
)

file(GLOB NIF_COMPOUNDS_HEADERS
	"${CMAKE_SOURCE_DIR}/lib/nif/include/gen/*.h"
)

file(GLOB NIF_OBJS_HEADERS
	"${CMAKE_SOURCE_DIR}/lib/nif/include/obj/*.h"
)

file(GLOB NIF_CUSTOM_TYPES_HEADERS
	"${CMAKE_SOURCE_DIR}/lib/nif/include/custom_types/*.h"
)

file(GLOB NIF_INTERFACES_HEADERS
	"${CMAKE_SOURCE_DIR}/lib/nif/include/interfaces/*.h"
)

file(GLOB NIF_GAME_SPECIFIC_HEADERS
	"${CMAKE_SOURCE_DIR}/lib/nif/include/game_specific/*.h"
)

add_definitions(-DNIFLIB_STATIC_LINK)

add_library(
	niflib
	STATIC 
	IMPORTED GLOBAL
)

set_target_properties(
	niflib 
	PROPERTIES IMPORTED_LOCATION             
	"${CMAKE_SOURCE_DIR}/lib/nif/release/niflib.lib"
)

set_target_properties(
	niflib
	PROPERTIES IMPORTED_LOCATION_DEBUG             
	"${CMAKE_SOURCE_DIR}/lib/nif/debug/niflib.lib"
)

#FBX SDK

set(FBXSDK_INCLUDES_PATH
	"${CMAKE_SOURCE_DIR}/lib/fbx/include"
)

add_library(
	fbxsdk
	STATIC 
	IMPORTED GLOBAL
)

set_target_properties(
	fbxsdk 
	PROPERTIES IMPORTED_LOCATION             
	"${CMAKE_SOURCE_DIR}/lib/fbx/lib/vs2015/x86/release/libfbxsdk.lib"
)

set_target_properties(
	fbxsdk
	PROPERTIES IMPORTED_LOCATION_DEBUG             
	"${CMAKE_SOURCE_DIR}/lib/fbx/lib/vs2015/x86/debug/libfbxsdk.lib"
)

add_definitions(-DFBXSDK_SHARED)

# BSA

set(LIBBSA_INCLUDES_PATH
	"${CMAKE_SOURCE_DIR}/lib/bsa/include"
)

add_library(
	libbsa
	STATIC 
	IMPORTED GLOBAL
)

set_target_properties(
	libbsa 
	PROPERTIES IMPORTED_LOCATION             
	"${CMAKE_SOURCE_DIR}/lib/bsa/lib/x86/release/bsa.lib"
)

set_target_properties(
	libbsa
	PROPERTIES IMPORTED_LOCATION_DEBUG             
	"${CMAKE_SOURCE_DIR}/lib/bsa/lib/x86/debug/bsa.lib"
)

add_definitions(-DLIBBSA_STATIC)

#MODULES

#Geometry

set(MIKKTSPACE_INCLUDES "${CMAKE_SOURCE_DIR}/modules/geometry/mikktspace")
file(GLOB MIKKTSPACE_SRCS "${CMAKE_SOURCE_DIR}/modules/geometry/mikktspace/*.c" )

set(EIGEN_INCLUDES "${CMAKE_SOURCE_DIR}/modules/geometry/eigen")

set(LIBIGL_INCLUDES "${CMAKE_SOURCE_DIR}/modules/geometry/libigl/include")

set(BOUNDINGMESH_INCLUDES "${CMAKE_SOURCE_DIR}/modules/geometry/bounding-mesh/src/boundingmesh")
file(GLOB BOUNDINGMESH_SRCS "${CMAKE_SOURCE_DIR}/modules/geometry/bounding-mesh/src/boundingmesh/*.cpp" )
file(GLOB BOUNDINGMESH_THIRD_SRCS "${CMAKE_SOURCE_DIR}/modules/geometry/bounding-mesh/thirdparty/EigenQP.*" )

set(QUICKHULL_INCLUDES "${CMAKE_SOURCE_DIR}/modules/geometry/quickhull")
file(GLOB QUICKHULL_SRCS "${CMAKE_SOURCE_DIR}/modules/geometry/quickhull/*.cpp" )

set(VHACD_INCLUDES "${CMAKE_SOURCE_DIR}/modules/geometry/v-hacd/src/VHACD_Lib/public" )
include_directories("${CMAKE_SOURCE_DIR}/modules/geometry/v-hacd/src/VHACD_Lib/public" )


list(APPEND TEST_LIBRARIES ${HAVOK_LIBS})
list(APPEND PROJECT_LIBRARIES ${HAVOK_LIBS})
list(APPEND PROJECT_LIBRARIES DirectXTex)
list(APPEND PROJECT_LIBRARIES loki)
list(APPEND PROJECT_LIBRARIES cbash)
list(APPEND PROJECT_LIBRARIES niflib)
list(APPEND PROJECT_LIBRARIES fbxsdk)
list(APPEND PROJECT_LIBRARIES libbsa)
list(APPEND PROJECT_LIBRARIES vhacd)
list(APPEND TEST_INCLUDES ${HAVOK_INCLUDES_PATH})
if (HAVE_SPEEDTREE)
list(APPEND PROJECT_INCLUDES ${SPDT_INCLUDES_PATH})
endif(HAVE_SPEEDTREE)
list(APPEND PROJECT_INCLUDES ${DIRECTXTEX_INCLUDES_PATH})
list(APPEND PROJECT_INCLUDES ${HAVOK_INCLUDES_PATH})
list(APPEND PROJECT_INCLUDES ${HAVOK_COMPAT_INCLUDES_PATH})
list(APPEND PROJECT_INCLUDES ${LOKI_INCLUDES_PATH})
list(APPEND PROJECT_INCLUDES ${CBASH_INCLUDES_PATH})
list(APPEND PROJECT_INCLUDES ${BETH_ANIMDATA_PATH})
list(APPEND PROJECT_INCLUDES ${NIFLIB_INCLUDES_PATH})
list(APPEND PROJECT_INCLUDES ${FBXSDK_INCLUDES_PATH})
list(APPEND PROJECT_INCLUDES ${LIBBSA_INCLUDES_PATH})


# SOURCES

set (PROJECT_SRC "${CMAKE_SOURCE_DIR}/src/core/FBXCommon.cpp"
                 "${CMAKE_SOURCE_DIR}/src/core/hkclsnam.cpp"
                 "${CMAKE_SOURCE_DIR}/src/core/hkcrc.cpp"
                 "${CMAKE_SOURCE_DIR}/src/core/hkfutils.cpp"
                 "${CMAKE_SOURCE_DIR}/src/core/hkxcore.cpp"
                 "${CMAKE_SOURCE_DIR}/src/core/hkxpch.cpp"
                 "${CMAKE_SOURCE_DIR}/src/core/hkxutils.cpp"
                 "${CMAKE_SOURCE_DIR}/src/core/log.cpp"
                 "${CMAKE_SOURCE_DIR}/src/core/nifcore.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/games.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/FBXWrangler.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/HKXWrangler.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/NifFile.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/Image_Utils.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/RawModel.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/Fbx2Raw.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/geometry.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/MathHelper.cpp"
				 "${CMAKE_SOURCE_DIR}/src/core/EulerAngles.cpp")

set (PROJECT_HEADERS "${CMAKE_SOURCE_DIR}/include/AnimationExport.h"
                     "${CMAKE_SOURCE_DIR}/include/stdafx.h"
#CORE
                     "${CMAKE_SOURCE_DIR}/include/core/EulerAngles.h"
                     "${CMAKE_SOURCE_DIR}/include/core/FBXCommon.h"
                     "${CMAKE_SOURCE_DIR}/include/core/hkcrc.h"
                     "${CMAKE_SOURCE_DIR}/include/core/hkfutils.h"
                     "${CMAKE_SOURCE_DIR}/include/core/hkxcmd.h"
                     "${CMAKE_SOURCE_DIR}/include/core/hkxcmdver.h"
                     "${CMAKE_SOURCE_DIR}/include/core/hkxcore.h"
                     "${CMAKE_SOURCE_DIR}/include/core/hkxcore.h"
                     "${CMAKE_SOURCE_DIR}/include/core/hkxpch.h"
                     "${CMAKE_SOURCE_DIR}/include/core/hkxutils.h"
                     "${CMAKE_SOURCE_DIR}/include/core/log.h"
                     "${CMAKE_SOURCE_DIR}/include/core/MathHelper.h"
					 "${CMAKE_SOURCE_DIR}/include/core/games.h"
					 "${CMAKE_SOURCE_DIR}/include/core/bsa.h"
					 "${CMAKE_SOURCE_DIR}/include/core/HKXWrangler.h"
					 "${CMAKE_SOURCE_DIR}/include/core/FBXWrangler.h"
					 "${CMAKE_SOURCE_DIR}/include/core/Fbx2Raw.h"
					 "${CMAKE_SOURCE_DIR}/include/core/mathfu.h"
					 "${CMAKE_SOURCE_DIR}/include/core/stb_image.h"
					 "${CMAKE_SOURCE_DIR}/include/core/stb_image_write.h"
					 "${CMAKE_SOURCE_DIR}/include/core/Image_Utils.h"
					 "${CMAKE_SOURCE_DIR}/include/core/Fbx2Raw.h"
					 "${CMAKE_SOURCE_DIR}/include/core/NifFile.h"
					 "${CMAKE_SOURCE_DIR}/include/core/AnimationCache.h"
					 )
set (PROJECT_COMMANDS
					 "${CMAKE_SOURCE_DIR}/include/commands/ImportFbx.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/ImportSkin.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/ImportAnimation.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/ExportAnimation.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/ExportRig.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/ImportRig.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/RetargetSkin.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/AddNodesToSkeleton.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/ConvertNif.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/Geometry.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/ListCreatures.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/RetargetCreature.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/desaturateVC.h"
					 "${CMAKE_SOURCE_DIR}/include/commands/fixsse.h"
                     "${CMAKE_SOURCE_DIR}/include/commands/esp/Esp.h"
                     "${CMAKE_SOURCE_DIR}/include/commands/esp/io/EspWriter.h"
                     "${CMAKE_SOURCE_DIR}/include/commands/esp/io/EspTypes.h"
                     "${CMAKE_SOURCE_DIR}/include/commands/esp/data/EspForm.h" 
                     "${CMAKE_SOURCE_DIR}/include/commands/esp/data/EspTES4Form.h"
                     "${CMAKE_SOURCE_DIR}/include/commands/esp/data/EspFile.h"
#resource
				)
# Commands

file(GLOB_RECURSE COMMANDS_SRC
	"${CMAKE_SOURCE_DIR}/src/commands/*.cpp"
)

list (APPEND COMMANDS_SRC
	"${CMAKE_SOURCE_DIR}/src/core/hkxcmd.cpp"
)

list (APPEND PROJECT_SRC ${HAVOK_COMPAT_SRC})
#list (APPEND PROJECT_SRC ${COMMANDS_SRC})
list (APPEND PROJECT_HEADERS ${HAVOK_COMPAT_HEADERS})
list (APPEND PROJECT_HEADERS ${BETH_ANIMDATA_HEADERS})
list (APPEND PROJECT_HEADERS ${NIF_CORE_HEADERS})
list (APPEND PROJECT_HEADERS ${NIF_COMPOUNDS_HEADERS})
list (APPEND PROJECT_HEADERS ${NIF_OBJS_HEADERS})
list (APPEND PROJECT_HEADERS ${NIF_CUSTOM_TYPES_HEADERS})
list (APPEND PROJECT_HEADERS ${NIF_INTERFACES_HEADERS})
list (APPEND PROJECT_HEADERS ${NIF_GAME_SPECIFIC_HEADERS})

#submodules
list (APPEND PROJECT_INCLUDES ${EIGEN_INCLUDES})
list (APPEND PROJECT_INCLUDES ${MIKKTSPACE_INCLUDES})
list (APPEND PROJECT_INCLUDES ${LIBIGL_INCLUDES})
list (APPEND PROJECT_INCLUDES ${BOUNDINGMESH_INCLUDES})
list (APPEND PROJECT_INCLUDES ${QUICKHULL_INCLUDES})
list (APPEND PROJECT_INCLUDES ${VHACD_INCLUDES})

#submodules
list (APPEND PROJECT_SRC ${BOUNDINGMESH_SRCS})
list (APPEND PROJECT_SRC ${BOUNDINGMESH_THIRD_SRCS})
list (APPEND PROJECT_SRC ${QUICKHULL_SRCS})
list (APPEND PROJECT_SRC ${MIKKTSPACE_SRCS})

source_group("Havok Compat\\Headers" FILES ${HAVOK_COMPAT_HEADERS})
source_group("Havok Compat\\Sources" FILES ${HAVOK_COMPAT_SRC})
source_group("Header Files" FILES ${PROJECT_HEADERS})
source_group("Commands" FILES ${COMMANDS_SRC})
source_group("AnimationData\\Headers" FILES ${BETH_ANIMDATA_HEADERS})
source_group("AnimationData\\Sources" FILES ${BETH_ANIMDATA_SRC})
source_group("NIFlib" FILES ${NIF_CORE_HEADERS})
source_group("NIFlib\\Compounds" FILES ${NIF_COMPOUNDS_HEADERS})
source_group("NIFlib\\Objects" FILES ${NIF_OBJS_HEADERS})
source_group("NIFlib\\CustomTypes" FILES ${NIF_CUSTOM_TYPES_HEADERS})
source_group("NIFlib\\Interfaces" FILES ${NIF_INTERFACES_HEADERS})
source_group("NIFlib\\GameSpecific" FILES ${NIF_GAME_SPECIFIC_HEADERS})
source_group("Geometry\\MikkTSpace" FILES ${MIKKTSPACE_SRCS})
source_group("Geometry\\BoundingMesh" FILES ${BOUNDINGMESH_SRCS})
source_group("Geometry\\QuickHull" FILES ${QUICKHULL_SRCS})
source_group("Textures\\DirectXTex" FILES ${DIRECTXTEX_INCLUDES_PATH})



# Settings when compiling with MSVC.
IF (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /D_SCL_SECURE_NO_WARNINGS=1 /D_CRT_NONSTDC_NO_DEPRECATE=1 /DUSE_EIGENQUADPROG")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
    # Update compiler flags.
    IF (PROJECT_STATIC_RUNTIME)
        FOREACH(flag
            CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG_INIT
            CMAKE_CXX_FLAGS_RELEASE  CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_DEBUG  CMAKE_CXX_FLAGS_DEBUG_INIT)
            STRING(REPLACE "/MD"  "/MT" "${flag}" "${${flag}}")
            SET("${flag}" "${${flag}} /EHsc ")
        ENDFOREACH()
    ENDIF ()

	#if(CMAKE_BUILD_TYPE MATCHES Debug)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD")
	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD")
	#endif(CMAKE_BUILD_TYPE MATCHES Debug)
	#WIN exclusive
	#find_library(SHLWAPI Shlwapi.lib)
	#find_library(WIN32_LEGACY legacy_stdio_definitions.lib)

ENDIF ()

add_subdirectory(${CMAKE_SOURCE_DIR}/modules/geometry/v-hacd/src)

IF (PROJECT_GUI)
	find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)

# Directory with the source code
	add_subdirectory(gui)
ENDIF ()

# Build core
add_executable				(ck-cmd ${PROJECT_SRC} ${PROJECT_HEADERS} ${PROJECT_COMMANDS} ${COMMANDS_SRC})
add_dependencies			(ck-cmd zlib docopt)
target_link_libraries		(ck-cmd ${PROJECT_LIBRARIES} ${ZLIB_LIBRARIES} ${DOCOPT_LIBRARIES} Shlwapi.lib legacy_stdio_definitions.lib)
if (HAVE_SPEEDTREE)
target_link_libraries		(ck-cmd ${SPEEDTREE_LIBS})
endif (HAVE_SPEEDTREE)
target_include_directories	(ck-cmd PUBLIC ${PROJECT_INCLUDES} ${DOCOPT_INCLUDE_DIRS})

add_custom_command(TARGET ck-cmd POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${PROJECT_SOURCE_DIR}/lib/fbx/lib/vs2015/x86/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:Release>/libfbxsdk.dll"
        $<TARGET_FILE_DIR:ck-cmd>)

# Build tester.
add_executable				(tests ${TEST_SRC})
add_dependencies			(tests googletest)
target_link_libraries		(tests ${GTEST_LIBRARIES} ${TEST_LIBRARIES})
target_include_directories	(tests PUBLIC ${TEST_INCLUDES})

message( "[MAIN]: checking libraries..." )