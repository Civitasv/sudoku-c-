cmake_minimum_required(VERSION 3.20)
project(c++_gui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTKMM REQUIRED gtkmm-3.0)
link_directories(
    ${GTKMM_LIBRARY_DIRS}  )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/builder.glade 
${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sudoku.css
${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

include_directories(${PROJECT_SOURCE_DIR})
add_executable(c++_gui main.cpp shuffle.cpp Sudoku.cpp sudoku_strategy.cpp)

target_link_libraries(c++_gui 
        ${GTKMM_LIBRARIES}  )