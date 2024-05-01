cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(-DUNICODE -D_UNICODE UNICODE _UNICODE)

project(libplumber)
project(plumber-cli)
project(plumber-gui)

add_subdirectory(externals)
add_subdirectory(libplumber)
add_subdirectory(plumber-cli)
add_subdirectory(plumber-gui)

set_target_properties(plumber-cli PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/plumber-cli)
set_target_properties(plumber-cli PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/out/plumber-cli)
set_target_properties(plumber-cli PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/out/plumber-cli)

set_target_properties(plumber-gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/plumber-gui)
set_target_properties(plumber-gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/out/plumber-gui)
set_target_properties(plumber-gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/out/plumber-gui)