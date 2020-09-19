cmake_minimum_required(VERSION 3.5)

project(qhttp LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QHTTP_DIR .)
set(QHTTP_SRC_DIR ${QHTTP_DIR}/src)
set(QHTTP_INC_DIR ${QHTTP_DIR}/include)
set(QHTTP_INC_DIR_2 ${QHTTP_INC_DIR}/qhttp)

set(CMAKE_INCLUDE_PATH 
    ${QHTTP_DIR}
    ${QHTTP_SRC_DIR}
    ${QHTTP_INC_DIR}
    ${QHTTP_INC_DIR_2}
    ${QHTTP_DIR}/3rdparty)

include_directories(
    ${QHTTP_DIR}
    ${QHTTP_SRC_DIR}
    ${QHTTP_INC_DIR}
    ${QHTTP_INC_DIR_2}
    ${QHTTP_DIR}/3rdparty)

set(SOURCES
    # Joyent http_parser
    ${QHTTP_DIR}/3rdparty/http-parser/http_parser.c
    
    ${QHTTP_SRC_DIR}/qhttpabstracts.cpp 
    ${QHTTP_SRC_DIR}/qhttpserverconnection.cpp 
    ${QHTTP_SRC_DIR}/qhttpserverrequest.cpp 
    ${QHTTP_SRC_DIR}/qhttpserverresponse.cpp 
    ${QHTTP_SRC_DIR}/qhttpserver.cpp
    )

set(HEADERS
    ${QHTTP_DIR}/3rdparty/http-parser/http_parser.h
    ${QHTTP_INC_DIR_2}/qhttpfwd.hpp 
    ${QHTTP_INC_DIR_2}/qhttpabstracts.hpp 
    ${QHTTP_INC_DIR_2}/qhttpheaders.hpp 
    ${QHTTP_INC_DIR_2}/qhttpserverconnection.hpp 
    ${QHTTP_INC_DIR_2}/qhttpserverrequest.hpp 
    ${QHTTP_INC_DIR_2}/qhttpserverresponse.hpp 
    ${QHTTP_INC_DIR_2}/qhttpserver.hpp
    )

find_package(Qt5 COMPONENTS Network WebSockets REQUIRED)
add_library(qhttp SHARED ${SOURCES} ${HEADERS})
target_link_libraries(qhttp Qt5::Core Qt5::Network Qt5::WebSockets )