include_directories(${Qt5Widgets_INCLUDE_DIRS} ${QtQml_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS} ${QtQml_DEFINITIONS} ${${Qt5Quick_DEFINITIONS}})

#qt5_add_resources(QT_RESOURCES qml.qrc)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(PROJECT "ck-gui")

project(${PROJECT})

if(NOT DEFINED HEADERS)
    file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
endif()

if(NOT DEFINED SOURCES)
    file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
endif()

file(GLOB_RECURSE UI_HAVOK_CLASSES_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/hkxclasses/*.h)
file(GLOB_RECURSE UI_HAVOK_CLASSES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/hkxclasses/*.cpp)


source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

source_group("Havok Compat\\Headers" FILES ${HAVOK_COMPAT_HEADERS})
source_group("Havok Compat\\Sources" FILES ${HAVOK_COMPAT_SRC})
source_group("UI Havok Classes\\Headers" FILES ${UI_HAVOK_CLASSES_HEADERS})
source_group("UI Havok Classes\\Sources" FILES ${UI_HAVOK_CLASSES_SOURCES})
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

	if(CMAKE_BUILD_TYPE MATCHES Debug)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD")
	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD")
	endif(CMAKE_BUILD_TYPE MATCHES Debug)

ENDIF ()

list(APPEND PROJECT_INCLUDES "${CMAKE_SOURCE_DIR}/gui")


add_executable(${PROJECT} ${HEADERS} ${SOURCES} ${QT_RESOURCES} ${PROJECT_INCLUDES} ${HAVOK_COMPAT_SRC} ${PROJECT_SRC})
target_include_directories	(${PROJECT}  PUBLIC ${PROJECT_INCLUDES} )
add_dependencies			(${PROJECT} zlib)
target_link_libraries(${PROJECT}
    Qt5::Widgets
	${PROJECT_LIBRARIES} ${ZLIB_LIBRARIES} Shlwapi.lib legacy_stdio_definitions.lib
    )

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${PROJECT_SOURCE_DIR}/../lib/fbx/lib/vs2015/x86/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:Release>/libfbxsdk.dll"
        $<TARGET_FILE_DIR:ck-gui>)


