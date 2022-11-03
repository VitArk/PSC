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
        src/widgets/ClickableLabel.cpp
        src/widgets/ClickableLabel.h
        src/widgets/DialWidget.cpp
        src/widgets/DialWidget.h
        src/widgets/ProtectionWidget.cpp
        src/widgets/ProtectionWidget.h
        src/widgets/SpinBoxDebounce.cpp
        src/widgets/SpinBoxDebounce.h
        src/widgets/DisplayWidget.cpp
        src/widgets/DisplayWidget.h
        src/widgets/PresetWidget.cpp
        src/widgets/PresetWidget.h
        src/widgets/ProtectionControlWidget.cpp
        src/widgets/ProtectionControlWidget.h src/widgets/ChannelsTrackingWidget.cpp src/widgets/ChannelsTrackingWidget.h src/widgets/OutputSwitch.cpp src/widgets/OutputSwitch.h src/widgets/StatusBar.cpp src/widgets/StatusBar.h)

#if (NOT CMAKE_PREFIX_PATH)
#    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
#            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
#endif ()


find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
