project ( sdc_group22 )

cmake_minimum_required( VERSION 2.6 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(Qt4 REQUIRED )
find_package(QCA2 REQUIRED)
find_package(ZeroCIce REQUIRED)
find_package(Threads REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

if (CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS( -Wall -Wundef -Wcast-align -Wchar-subscripts -Wpointer-arith
       -Wwrite-strings -Wpacked -Wformat-security -Wmissing-format-attribute
       -Wold-style-cast -Woverloaded-virtual -Wnon-virtual-dtor -Wextra -Werror)
endif(CMAKE_COMPILER_IS_GNUCXX)

add_subdirectory( src )
add_subdirectory( doc )
add_subdirectory( test )
