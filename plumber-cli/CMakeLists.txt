add_executable(plumber-cli CLIBoard.h CLIBoard.cpp Game.h Game.cpp cli_util.h cli_util.cpp main.cpp)
target_include_directories(plumber-cli PUBLIC ".")
target_link_libraries(plumber-cli PRIVATE libplumber color winmm.lib Audio)

add_custom_command(TARGET plumber-cli PRE_BUILD COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/out/plumber-cli/assets)
add_custom_command(TARGET plumber-cli POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/out/plumber-cli/assets)