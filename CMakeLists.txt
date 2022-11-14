cmake_minimum_required(VERSION 3.18)
project(PS-Management
        VERSION 1.3.1
        DESCRIPTION "Power Supply Management (PSM). GUI Qt-based application to control Programmable DC Power supplies using a SCPI based communication protocol, over RS232/USB interface."
        HOMEPAGE_URL "https://github.com/vitark/PS-Management"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(QT Qt${QT_VERSION})
set(REQUIRED_LIBS Core Gui Widgets Svg SerialPort)
set(REQUIRED_LIBS_QUALIFIED ${QT}::Core ${QT}::Gui ${QT}::Widgets ${QT}::Svg ${QT}::SerialPort)
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

string(TIMESTAMP TODAY "%Y%m%d")
string(TIMESTAMP YEAR "%Y")

set(target ${PROJECT_NAME})

# Default Qt5 path
if(WIN32)
    set(Qt5_DIR "d:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5/" CACHE PATH "directory where Qt5Config.cmake exists.")
    set(CMAKE_WIN32_EXECUTABLE ON)
elseif(APPLE)
    set(Qt5_DIR "/usr/local/Cellar/qt/5.12.0/clang_64/lib/cmake/Qt5/" CACHE PATH "directory where Qt5Config.cmake exists.")
    set(CMAKE_MACOSX_BUNDLE ON)
else()
    set(Qt5_DIR "" CACHE PATH "directory where Qt5Config.cmake exists.")
endif()

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR} # config.h
)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/assets/windows.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/windows.rc
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Global.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/resources.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Communication.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/Messages.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Settings.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/UTP3305C.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/UTP3303C.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/BaseSCPI.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/Factory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CommunicationMetrics.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ClickableLabel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/DialWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ProtectionWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/SpinBoxDebounce.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/DisplayWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/PresetWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ProtectionControlWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ChannelsTrackingWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/OutputSwitch.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/StatusBar.h
        )

set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Communication.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Settings.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/Factory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ClickableLabel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/DialWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ProtectionWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/SpinBoxDebounce.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/DisplayWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/PresetWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ProtectionControlWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ChannelsTrackingWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/OutputSwitch.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/StatusBar.cpp
        )

set(ICON_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/resources.qrc)
qt5_add_resources(ICON_RESOURCE_ADDED ${ICON_RESOURCE})

if(UNIX AND NOT APPLE)

elseif(WIN32)
    list(APPEND ICON_RESOURCE_ADDED "${CMAKE_CURRENT_BINARY_DIR}/windows.rc")
elseif(APPLE)
    set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/src/assets/power-supply.icns)
    list(APPEND ICON_RESOURCE_ADDED ${APP_ICON})
endif()


add_executable(${target}
        ${HEADER}
        ${SOURCE}
        ${ICON_RESOURCE_ADDED}
        )

target_link_libraries(${target} ${REQUIRED_LIBS_QUALIFIED})
set_target_properties(${target} PROPERTIES AUTORCC_OPTIONS "--compress;9")

if(UNIX AND NOT APPLE)
    install(TARGETS ${target} RUNTIME DESTINATION bin)
elseif(WIN32)
    install(TARGETS ${target} RUNTIME DESTINATION bin)
elseif(APPLE)
    set(CMAKE_MACOSX_BUNDLE ON)
    set(CMAKE_MACOSX_RPATH ON)
    set(MACOSX_BUNDLE_ICON_FILE power-supply.icns)

    install(TARGETS ${target} BUNDLE DESTINATION .
            RUNTIME DESTINATION .)

    set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_target_properties(${target}
            PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
            MACOSX_BUNDLE_INFO_STRING "${PROJECT_DESCRIPTION} Copyright (c) 2021-${YEAR} VitArk"
            MACOSX_BUNDLE_ICON_FILE power-supply.icns
            MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}"
            MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
            MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
            MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2021-${YEAR} VitArk"
            )
endif()


#set(desktop.path applications)
#set(desktop.files resources/example.desktop)
#set(icon.path icons/hicolor/64x64/apps)
#set(icon.files resources/icon64.png)
#set(iconsvg.path icons/hicolor/scalable/apps)
#set(iconsvg.files resources/icon.svg)
#foreach(items IN ITEMS desktop icon iconsvg)
#    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${${items}.files}
#            DESTINATION share/${${items}.path}
#            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
#endforeach()
# Components:
if(CMAKE_BUILD_TYPE_UPPER MATCHES "^(DEBUG|RELWITHDEBINFO)$")
    set(CPACK_STRIP_FILES FALSE)
else()
    set(CPACK_STRIP_FILES TRUE)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/Packaging.cmake)

#---------------------------------------------------------------------------------

option(CMake_RUN_CLANG_TIDY "Run clang-tidy with the compiler." OFF)
if(CMake_RUN_CLANG_TIDY)
    if(CMake_SOURCE_DIR STREQUAL CMake_BINARY_DIR)
        message(FATAL_ERROR "CMake_RUN_CLANG_TIDY requires an out-of-source build!")
    endif()
    find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)
    if(NOT CLANG_TIDY_COMMAND)
        message(WARNING "CMake_RUN_CLANG_TIDY is ON but clang-tidy is not found!")
        set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE)
    else()
        set(CLANG_TIDY_CHECKS "-*,modernize-*")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND};-checks=${CLANG_TIDY_CHECKS};-header-filter='${CMAKE_SOURCE_DIR}/src/*'")
    endif()

    # Create a preprocessor definition that depends on .clang-tidy content so
    # the compile command will change when .clang-tidy changes.  This ensures
    # that a subsequent build re-runs clang-tidy on all sources even if they
    # do not otherwise need to be recompiled.  Nothing actually uses this
    # definition.  We add it to targets on which we run clang-tidy just to
    # get the build dependency on the .clang-tidy file.
    file(SHA1 ${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy clang_tidy_sha1)
    set(CLANG_TIDY_DEFINITIONS "CLANG_TIDY_SHA1=${clang_tidy_sha1}")
    unset(clang_tidy_sha1)

endif()

option(CMake_RUN_IWYU "Run include-what-you-use with the compiler." OFF)
if(CMake_RUN_IWYU)
    find_program(IWYU_COMMAND NAMES include-what-you-use iwyu)
    if(NOT IWYU_COMMAND)
        message(WARNING "CMake_RUN_IWYU is ON but include-what-you-use is not found!")
    else()
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE
                "${IWYU_COMMAND};-Xiwyu;--mapping_file=${CMake_SOURCE_DIR}/Utilities/IWYU/mapping.imp;-w")
        list(APPEND CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${CMake_IWYU_OPTIONS})
    endif()
endif()

# Clazy is a Qt oriented code checker based on clang framework. Krazy's little brother.
set(CMake_RUN_CLAZY OFF CACHE BOOL "Add clazy check for builds")
if(ENABLE_CLAZY)
    find_program(CLAZY_EXECUTABLE NAMES clazy PATHS /usr/local/llvm/bin /usr/local/bin /opt/clazy/bin)
    if(CLAZY_EXECUTABLE)
        message(STATUS "clazy found: ${CLAZY_EXECUTABLE}")
    else()
        message(AUTHOR_WARNING "clazy not found.")
    endif()
endif()
