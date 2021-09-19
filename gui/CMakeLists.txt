include_directories(${Qt5Widgets_INCLUDE_DIRS} ${QtQml_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS} ${QtQml_DEFINITIONS} ${${Qt5Quick_DEFINITIONS}})

#qt5_add_resources(QT_RESOURCES qml.qrc)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PROJECT "ck-gui")

project(${PROJECT})


# QT Advanced Dock Widgets

set ( QTADS_INCLUDES_PATH "${CMAKE_SOURCE_DIR}/lib/QTADS/include")

add_library(
	QTADS
	STATIC 
	IMPORTED GLOBAL
)

set_target_properties(
	QTADS 
	PROPERTIES IMPORTED_LOCATION
	"${CMAKE_SOURCE_DIR}/lib/QTADS/lib/Release/qtadvanceddocking.lib"
)

set_target_properties(
	QTADS
	PROPERTIES IMPORTED_LOCATION_DEBUG             
	"${CMAKE_SOURCE_DIR}/lib/QTADS/lib/Debug/qtadvanceddocking.lib"
)

set (GUI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h"
                 "${CMAKE_CURRENT_SOURCE_DIR}/src/workspace.h"
                 "${CMAKE_CURRENT_SOURCE_DIR}/src/ILogger.h"
                 "${CMAKE_CURRENT_SOURCE_DIR}/src/log.h")

set (GUI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp"
                 "${CMAKE_CURRENT_SOURCE_DIR}/src/workspace.cpp"
                 "${CMAKE_CURRENT_SOURCE_DIR}/src/log.cpp")

if(NOT DEFINED AnimData)
    file(GLOB_RECURSE ANIMDATA_HEADERS ${CMAKE_SOURCE_DIR}/gui/src/animdata/*.h)
    file(GLOB_RECURSE ANIMDATA_SOURCES ${CMAKE_SOURCE_DIR}/gui/src/animdata/*.cpp)

    source_group("Animation Data\\Headers" FILES ${ANIMDATA_HEADERS})
    source_group("Animation Data\\Sources" FILES ${ANIMDATA_SOURCES})
endif()  

if(NOT DEFINED Havok)
    file(GLOB_RECURSE HKX_HEADERS ${CMAKE_SOURCE_DIR}/gui/src/hkx/*.h)
    file(GLOB_RECURSE HKX_SOURCES ${CMAKE_SOURCE_DIR}/gui/src/hkx/*.cpp)

    source_group("Havok\\Headers" FILES ${HKX_HEADERS})
    source_group("Havok\\Sources" FILES ${HKX_SOURCES})
endif()   

if(NOT DEFINED MODELS)
    file(GLOB_RECURSE MODEL_HEADERS ${CMAKE_SOURCE_DIR}/gui/src/models/*.h)
    file(GLOB_RECURSE MODEL_SOURCES ${CMAKE_SOURCE_DIR}/gui/src/models/*.cpp)

    source_group("Models\\Headers" FILES ${MODEL_HEADERS})
    source_group("Models\\Sources" FILES ${MODEL_SOURCES})
endif()   

if(NOT DEFINED WIDGETS)
    file(GLOB_RECURSE WIDGET_HEADERS ${CMAKE_SOURCE_DIR}/gui/src/widgets/*.h)
    file(GLOB_RECURSE WIDGET_SOURCES ${CMAKE_SOURCE_DIR}/gui/src/widgets/*.cpp)
    file(GLOB_RECURSE WIDGET_UIS ${CMAKE_SOURCE_DIR}/gui/src/widgets/*.ui)

    source_group("Widgets\\Headers" FILES ${WIDGET_HEADERS})
    source_group("Widgets\\Sources" FILES ${WIDGET_SOURCES})
    source_group("Widgets\\UI" FILES ${WIDGET_UIS})
endif()

if(NOT DEFINED WINDOWS)
    file(GLOB_RECURSE WINDOW_HEADERS ${CMAKE_SOURCE_DIR}/gui/src/windows/*.h)
    file(GLOB_RECURSE WINDOW_SOURCES ${CMAKE_SOURCE_DIR}/gui/src/windows/*.cpp)
    file(GLOB_RECURSE WINDOW_UIS ${CMAKE_SOURCE_DIR}/gui/src/windows/*.ui)

    source_group("Windows\\Headers" FILES ${WINDOW_HEADERS})
    source_group("Windows\\Sources" FILES ${WINDOW_SOURCES})
    source_group("Windows\\UI" FILES ${WINDOW_UIS})
endif()

file(GLOB_RECURSE QTADS_HEADERS ${QTADS_INCLUDES_PATH}/*.h)

list(APPEND PROJECT_INCLUDES "${CMAKE_SOURCE_DIR}/gui") 
list(APPEND PROJECT_INCLUDES "${QTADS_INCLUDES_PATH}")

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

add_library( gui-lib STATIC 
    ${GUI_HEADERS} 
    ${GUI_SOURCES}
    ${MODEL_HEADERS}
    ${MODEL_SOURCES}
    ${HKX_HEADERS}
    ${HKX_SOURCES}
    ${ANIMDATA_HEADERS}
    ${ANIMDATA_SOURCES}
)
target_include_directories	(gui-lib  PUBLIC ${PROJECT_INCLUDES} )

add_executable(${PROJECT} 
    ${WINDOW_HEADERS} 
    ${WINDOW_SOURCES} 
    ${WINDOW_UIS}
    ${WIDGET_HEADERS} 
    ${WIDGET_SOURCES}
    ${WIDGET_UIS}
    ${QT_RESOURCES} 
    "${CMAKE_SOURCE_DIR}/gui/src/main.cpp" 
$<TARGET_OBJECTS:ck-cmd-lib>)
target_include_directories	(${PROJECT}  PUBLIC ${PROJECT_INCLUDES} )
add_dependencies			(${PROJECT} zlib)
add_dependencies			(${PROJECT} docopt_parser)
target_link_libraries(${PROJECT}
    ${DOCOPT_LIBRARIES}   
    ck-cmd-lib
    gui-lib
    FSM-lib
    QTADS
    Qt5::Widgets
    ${OPENGL_LIBRARIES}
	${PROJECT_LIBRARIES} ${ZLIB_LIBRARIES} Shlwapi.lib legacy_stdio_definitions.lib
    )

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${PROJECT_SOURCE_DIR}/../lib/fbx/lib/vs2015/x86/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:Release>/libfbxsdk.dll"
        $<TARGET_FILE_DIR:ck-gui>)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${PROJECT_SOURCE_DIR}/../lib/SpeedTreeRT/SpeedTreeRT.dll"
        $<TARGET_FILE_DIR:ck-gui>)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/../vendor/havok_2010_2_0/VisualDebugger/hkVisualDebugger.exe"
        $<TARGET_FILE_DIR:ck-gui>)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${PROJECT_SOURCE_DIR}/../lib/QTADS/lib/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:Release>/qtadvanceddocking.dll"
        $<TARGET_FILE_DIR:ck-gui>)

if(TARGET Qt5::windeployqt)
    # execute windeployqt in a tmp directory after build
    add_custom_command(TARGET ${PROJECT}
        POST_BUILD
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
        COMMAND Qt5::windeployqt $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>$<$<CONFIG:RelWithDebInfo>:--release> --dir "$<TARGET_FILE_DIR:ck-gui>" "$<TARGET_FILE_DIR:ck-gui>/$<TARGET_FILE_NAME:ck-gui>"
    )
endif()


set (GUI_TEST_SRC "${CMAKE_SOURCE_DIR}/gui/test/test.cpp")

# Build tester.
add_executable				(gui-tests ${GUI_TEST_SRC} $<TARGET_OBJECTS:ck-cmd-lib>)
add_dependencies			(gui-tests googletest)
add_dependencies			(gui-tests zlib)
add_dependencies			(gui-tests docopt_parser)
target_link_libraries		(gui-tests ${GTEST_LIBRARIES} ${TEST_LIBRARIES}
    ${DOCOPT_LIBRARIES}
    ck-cmd-lib
    gui-lib
    FSM-lib
    QTADS
    Qt5::Widgets
	${PROJECT_LIBRARIES} ${ZLIB_LIBRARIES} Shlwapi.lib legacy_stdio_definitions.lib
    )
target_include_directories	(gui-tests PUBLIC ${TEST_INCLUDES} ${PROJECT_INCLUDES})
enable_testing()
add_test(GuiTests gui-tests)


