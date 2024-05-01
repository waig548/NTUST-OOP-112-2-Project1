add_executable(plumber-gui gui.h gui.cpp GUIBoard.h GUIBoard.cpp main.cpp)
target_include_directories(plumber-gui PUBLIC ".")
target_link_libraries(plumber-gui PRIVATE glad glfw imgui Audio stb libplumber)

add_custom_command(TARGET plumber-gui PRE_BUILD COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/out/plumber-gui/assets)
add_custom_command(TARGET plumber-gui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/out/plumber-gui/assets)