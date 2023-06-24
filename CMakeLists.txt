cmake_minimum_required(VERSION 3.25)
project(QVoltMeter)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Charts
        REQUIRED)
find_package(Poco COMPONENTS
        Data
        DataSQLite
        REQUIRED)
find_package(CSerialPort REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(QVoltMeter src/main.cpp Forms/qvoltmeter.cpp Forms/qvoltmeter.h Forms/qvoltmeter.ui include/VoltMeterSession.h src/VoltMeterSession.cpp include/SQLHandler.h src/SQLHandler.cpp include/SQLVoltRecord.h)
target_link_libraries(QVoltMeter
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Charts
        Poco::Data
        Poco::DataSQLite
        ${CSerialPort_LIBRARY}
        spdlog::spdlog
        )
target_include_directories(QVoltMeter PRIVATE include Forms)
target_include_directories(QVoltMeter PRIVATE ${CSerialPort_INCLUDE_DIR})

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
