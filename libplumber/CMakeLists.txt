add_library(libplumber Vec.h Vec.cpp Board.h Board.cpp audio_sys.h audio_sys.cpp  util.h util.cpp)
target_include_directories(libplumber PUBLIC ".")
target_link_libraries(libplumber PRIVATE Audio)