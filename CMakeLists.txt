CMAKE_MINIMUM_REQUIRED(VERSION 3.22)

project(VoltMeter CXX)
find_package(MFC REQUIRED)
find_package(CSerialPort REQUIRED)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2) # 1 the static MFC library 2 shared MFC library

include_directories(${CMAKE_SOURCE_DIR}/include)
FIlE(GLOB SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CSerialPort_LIBRARY})