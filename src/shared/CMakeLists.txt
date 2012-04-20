include_directories( ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/.. ${QCA2_INCLUDE_DIR} )

#next lines are for creating .h & .cpp files out of .ice Files
set(out_dir "${CMAKE_CURRENT_BINARY_DIR}")
set(out_file "${out_dir}/SecureDistributedChat.cpp")

add_custom_command(OUTPUT
"${out_file}"
 COMMAND "${ZeroCIce_slice2cpp_BIN}"
   --add-header=cstddef
   -I /usr/share/Ice/slice/
   -I /usr/share/Ice-3.4.1/slice/
   -I /usr/share/slice/
   --stream
   --output-dir "${out_dir}"
   "${CMAKE_CURRENT_SOURCE_DIR}/SecureDistributedChat.ice"
)

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

set( SHARED_SRC
     sdcHelper.cpp
     Security.cpp
     ${out_file}
)

add_library(Shared ${SHARED_SRC})
