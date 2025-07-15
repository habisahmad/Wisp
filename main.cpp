#include "editor_window.hpp"

int main(int argc, char** argv){
    textbuf = new Fl_Text_Buffer;
    Fl_Window* window = new MyEditor(640, 400, "Text Editor");

    window->show(1, argv);
    if (argc > 1) load_file(argv[1], -1);

    return Fl::run();
}