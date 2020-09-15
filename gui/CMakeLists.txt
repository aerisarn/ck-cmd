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
source_group("UI Havok Classes\\Headers" FILES ${UI_HAVOK_CLASSES_HEADERS})
source_group("UI Havok Classes\\Sources" FILES ${UI_HAVOK_CLASSES_SOURCES})
source_group("Header Files" FILES ${PROJECT_HEADERS})
source_group("AnimationData\\Headers" FILES ${BETH_ANIMDATA_HEADERS})
source_group("AnimationData\\Sources" FILES ${BETH_ANIMDATA_SRC})



# Settings when compiling with MSVC.
IF (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /D_SCL_SECURE_NO_WARNINGS=1 /D_CRT_NONSTDC_NO_DEPRECATE=1 /DUSE_EIGENQUADPROG /DM_E=2.71828182845904523536 /DM_LOG2E=1.44269504088896340736 /DM_LOG10E=0.434294481903251827651 /DM_LN2=0.693147180559945309417 /DM_LN10=2.30258509299404568402 /DM_PI=3.14159265358979323846 /DM_PI_2=1.57079632679489661923 /DM_PI_4=0.785398163397448309616 /DM_1_PI=0.318309886183790671538 /DM_2_PI=0.636619772367581343076 /DM_2_SQRTPI=1.12837916709551257390 /DM_SQRT2=1.41421356237309504880 /DM_SQRT1_2=0.707106781186547524401 /D_MATH_DEFINES_DEFINED")
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

list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/gui/test/test.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/gui/src/main.cpp")

add_library( gui-lib STATIC ${HEADERS} ${SOURCES} ${QT_RESOURCES} )
target_include_directories	(gui-lib  PUBLIC ${PROJECT_INCLUDES} )

add_executable(${PROJECT} "${CMAKE_SOURCE_DIR}/gui/src/main.cpp" $<TARGET_OBJECTS:ck-cmd-lib>)
target_include_directories	(${PROJECT}  PUBLIC ${PROJECT_INCLUDES} )
add_dependencies			(${PROJECT} zlib)
target_link_libraries(${PROJECT}
    ${DOCOPT_LIBRARIES}   
    ck-cmd-lib
    gui-lib
    Qt5::Widgets
	${PROJECT_LIBRARIES} ${ZLIB_LIBRARIES} Shlwapi.lib legacy_stdio_definitions.lib
    )

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${PROJECT_SOURCE_DIR}/../lib/fbx/lib/vs2015/x86/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:Release>/libfbxsdk.dll"
        $<TARGET_FILE_DIR:ck-gui>)


set (GUI_TEST_SRC "${CMAKE_SOURCE_DIR}/gui/test/test.cpp")

list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/gui/src/main.cpp")

# Build tester.
add_executable				(gui-tests ${GUI_TEST_SRC} $<TARGET_OBJECTS:ck-cmd-lib>)
add_dependencies			(gui-tests googletest)
add_dependencies			(gui-tests zlib)
target_link_libraries		(gui-tests ${GTEST_LIBRARIES} ${TEST_LIBRARIES}
    ${DOCOPT_LIBRARIES}
    ck-cmd-lib
    gui-lib
    Qt5::Widgets
	${PROJECT_LIBRARIES} ${ZLIB_LIBRARIES} Shlwapi.lib legacy_stdio_definitions.lib
    )
target_include_directories	(gui-tests PUBLIC ${TEST_INCLUDES} ${PROJECT_INCLUDES})
enable_testing()
add_test(GuiTests gui-tests)


