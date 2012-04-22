project ( sdc_group22 )

cmake_minimum_required( VERSION 2.6 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package( Qt4 COMPONENTS QtCore QtTest QtSql QtGui REQUIRED )

find_package(QCA2 REQUIRED)
find_package(ZeroCIceCore REQUIRED)
find_package(ZeroCIceUtil REQUIRED)
find_package(ZeroCIceExecutables REQUIRED)
find_package(Threads REQUIRED)

IF(ZeroCIce_slice2cpp_FOUND)
    message(STATUS "slice2cpp found")
ENDIF(ZeroCIce_slice2cpp_FOUND)

if (CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS( -Wall -Wundef -Wcast-align -Wchar-subscripts -Wpointer-arith
       -Wwrite-strings -Wpacked -Wformat-security -Wmissing-format-attribute
       -Wold-style-cast -Woverloaded-virtual -Wnon-virtual-dtor )
endif(CMAKE_COMPILER_IS_GNUCXX)

add_subdirectory( src )
add_subdirectory( doc )
