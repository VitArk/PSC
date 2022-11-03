cmake_minimum_required(VERSION 3.18)
project(PS-Management)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets Svg SerialPort)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Svg Qt5::SerialPort)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/MainWindow.ui
        src/Application.cpp
        src/Application.h
        src/Global.h
        src/resources.qrc
        src/Communication.cpp
        src/Communication.h
        src/protocol/Messages.h
        src/Settings.cpp
        src/Settings.h
        src/protocol/UTP3305C.h
        src/protocol/UTP3303C.h
        src/protocol/BaseSCPI.h
        src/protocol/Factory.h
        src/protocol/Factory.cpp
        src/CommunicationMetrics.h
        src/ClickableLabel.cpp
        src/ClickableLabel.h
        src/DialWidget.cpp
        src/DialWidget.h
        src/ProtectionWidget.cpp
        src/ProtectionWidget.h
        src/SpinBoxDebounce.cpp
        src/SpinBoxDebounce.h
        src/DisplayWidget.cpp
        src/DisplayWidget.h
        src/PresetWidget.cpp
        src/PresetWidget.h
        src/ProtectionControlWidget.cpp
        src/ProtectionControlWidget.h src/ChannelsTrackingWidget.cpp src/ChannelsTrackingWidget.h src/OutputSwitch.cpp src/OutputSwitch.h)

#if (NOT CMAKE_PREFIX_PATH)
#    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
#            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
#endif ()


find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
