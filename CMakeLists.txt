CMAKE_MINIMUM_REQUIRED(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)

project(VoltMeter)
find_package(MFC REQUIRED)
find_package(CSerialPort REQUIRED)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2) # 1 the static MFC library 2 shared MFC library

set(VoltMeter_SRCs src/pch.cpp src/VoltMeterSession.cpp src/VoltMeter.rc src/VoltMeterDlg.cpp src/VoltMeter.cpp)

add_executable(${PROJECT_NAME} WIN32 ${VoltMeter_SRCs})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CSerialPort_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${CSerialPort_LIBRARY} setupapi)
