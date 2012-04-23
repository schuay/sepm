find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_subdirectory( client )
add_subdirectory( shared )

#add_subdirectory( server )
