# PSM
Power Supply Management (PSM). GUI Qt-based application for remote control of UNI-T UTP330xC Series Programmable DC Power supplies via RS232/USB interface.



#internal build
Windows:
x86
 - cmake -DQt5_DIR="d:/Qt/5.15.2/msvc2019/lib/cmake/Qt5" -G "Visual Studio 16 2019" -A Win32  ../
 - cmake --build . --target bundle --config Release
 
x64
 - cmake -DQt5_DIR="d:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5"  ../
 - cmake --build . --target bundle --config Release
