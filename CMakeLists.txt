cmake_minimum_required(VERSION 3.10)
project(Wisp)

set(CMAKE_CXX_STANDARD 17)

find_package(FLTK REQUIRED NO_MODULE)

add_executable(my_text_editor
    main.cpp
    editor_window.cpp
)

target_link_libraries(my_text_editor PRIVATE FLTK::FLTK)
