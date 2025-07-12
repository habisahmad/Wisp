#include "editor_window.hpp"
#include <cstring> 
#include <cerrno>
#include <stdlib.h>

int changed = 0;
int loading = 0;
char filename[256] = "";
char title[256] = "";
Fl_Text_Buffer* textbuf;

MyEditor::MyEditor(int w, int h, const char* t) : Fl_Double_Window(w, h, t) {
    textbuf = new Fl_Text_Buffer();
    editor = new Fl_Text_Editor(0, 30, 640, 370);
    editor->buffer(textbuf);

    // Keeps track of changes to a file
    textbuf->add_modify_callback(changed_cb, this);
    textbuf->call_modify_callbacks();

    // Define font
    editor->textfont(FL_COURIER);
    Fl_Menu_Bar *m = new Fl_Menu_Bar(0, 0, 640, 30);
    m->add("&File/New File", 0, (Fl_Callback *)new_cb);
    m->add("&File/Open File...", FL_CTRL + 'o', (Fl_Callback *)open_cb);
    m->add("&File/Save File...", FL_CTRL + 's', (Fl_Callback *)save_cb);
    m->add("&File/Save File As...", FL_CTRL + FL_SHIFT + 's', (Fl_Callback *)saveas_cb);
    m->add("&File/Exit", FL_CTRL + 'q', (Fl_Callback *)quit_cb, 0);

    m->add("&Edit/Cut", FL_CTRL + 'x', (Fl_Callback *)cut_cb, this);
    m->add("&Edit/Copy", FL_CTRL + 'c', (Fl_Callback *)copy_cb, this);
    m->add("&Edit/Paste", FL_CTRL + 'v', (Fl_Callback *)paste_cb, this);
    m->add("&Edit/&Delete", 0, (Fl_Callback *)delete_cb, this);

    m->add("&Search/&Find", FL_CTRL + 'f', (Fl_Callback *)find_cb, this);
    m->add("&Search/&Find Again", FL_CTRL + 'g', (Fl_Callback *)find2_cb, this);
    m->add("&Search/&Replace", FL_CTRL + 'r', (Fl_Callback *)replace_cb, this);
    m->add("&Search/&Replace Again", FL_CTRL + 't', (Fl_Callback *)replace2_cb, this);
    // Replace dialog
    replace_dlg = new Fl_Window(300, 105, "Replace");
    replace_find = new Fl_Input(70, 10, 200, 5, "Find:");
    replace_with = new Fl_Input(70, 40, 200, 25, "Replace:");
    replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    replace_next = new Fl_Return_Button(105, 70, 120, 25, "Replace Next");
    replace_cancel = new Fl_Button(230, 60, 70, 25, "Cancel");
}


MyEditor::~MyEditor() {
}

void changed_cb(int, int nInserted, int nDeleted,int, const char*, void* v) {
    // Change changed variable to 1
    if ((nInserted || nDeleted) && !loading) changed = 1;
    MyEditor *w = (MyEditor*)v;
    // set title to changed
    set_title(w);
    if (loading) w->editor->show_insert_position(); 
}

// Call kf_copy to copy to clipboard
void copy_cb(Fl_Widget*, void* v) {
    MyEditor* e = (MyEditor*)v;
    Fl_Text_Editor::kf_copy(0, e->editor);
}

void cut_cb(Fl_Widget*, void* v) {
    MyEditor* e = (MyEditor*)v;
    Fl_Text_Editor::kf_cut(0, e->editor);
}

void delete_cb(Fl_Widget*, void* v) {
    textbuf->remove_selection();
}

void find_cb(Fl_Widget*, void* v){
    MyEditor* e = (MyEditor*)v;
    // Copy User Input to val
    const char* val; 
    val = fl_input("Search String:", e->search);
    // If user entered a string
    if (val){
        strcpy(e->search, val);
        find2_cb(nullptr, v);
    }
}

void find2_cb(Fl_Widget*, void* v) {
    MyEditor* e = (MyEditor*)v;

    if (e->search[0] == '\0'){
        // If string is blank get another
        find_cb(nullptr, v);
        return;
    }
    int pos = e->editor->insert_position();
    int found = textbuf->search_forward(pos, e->search, &pos);
    if (found) {
        textbuf->select(pos, pos+strlen(e->search));
        e->editor->insert_position(pos+strlen(e->search));
        e->editor->show_insert_position();

    }
    else fl_alert("No occurences found");
}

// clears the text of editor
void new_cb(Fl_Widget*, void*) {
    // gives user a chance to save
    if (!check_save()) return;

    filename[0] = '\0';
    textbuf->select(0, textbuf->length());
    textbuf->remove_selection();
    changed = 0;
    textbuf->call_modify_callbacks();
}

void open_cb(Fl_Widget*, void*){
    if (!check_save()) return;

    char *newfile = fl_file_chooser("Open File?", "*", filename);
    if (newfile != NULL) load_file(newfile, -1);
}

void paste_cb(Fl_Widget*, void* v) {
    MyEditor* e = (MyEditor*) v;
    Fl_Text_Editor::kf_paste(0, e->editor);
}

void quit_cb(Fl_Widget*, void*) {
    // if changed, give a chance to save
    if (changed && !check_save()){
        return;
    }
    exit(0);
}

void replace_cb(Fl_Widget*, void* v){
    MyEditor* e = (MyEditor*)v;
    e->replace_dlg->show();
}

void replace2_cb(Fl_Widget*, void* v){
    MyEditor* e = (MyEditor*) v;
    const char *find = e->replace_find->value();
    const char *replace = e->replace_with->value();

    // Search for another string if cannot find.
    if (find[0] == '\0'){
        e->replace_dlg->show();
        return;
    }
    e->replace_dlg->hide();

    int pos = e->editor->insert_position();
    int found = textbuf->search_forward(pos, find, &pos);

    if (found) {
        textbuf->select(pos, pos+strlen(find));
        textbuf->remove_selection();
        textbuf->insert(pos, replace);
        textbuf->select(pos, pos+strlen(replace));
        e->editor->insert_position(pos+strlen(replace));
        e->editor->show_insert_position();
    }
    else fl_alert("No occurences of \'%s\' found.", find);
}

void replall_cb(Fl_Widget*, void* v){
    MyEditor *e = (MyEditor*) v;
    const char *find = e->replace_find->value();
    const char *replace = e->replace_with->value();

    find = e->replace_find->value();
    if (find[0] == '\0'){
        e->replace_dlg->show();
        return;
    }
    e->replace_dlg->hide();
    e->editor->insert_position(0);

    int times = 0;
    for (int found = 1; found; ){
        int pos = e->editor->insert_position();
        found = textbuf->search_forward(pos, find, &pos);

        if (found) {
            textbuf->select(pos, pos+strlen(find));
            textbuf->remove_selection();
            textbuf->insert(pos, replace);
            e->editor->insert_position(pos+strlen(replace));
            e->editor->show_insert_position();
            times++;
        }
        if (times) fl_message("Repalced %d occurences", times);
        else fl_alert("No occurences of \'%s\' found.", find);
    }
}

void replcan_cb(Fl_Widget*, void* v){
    MyEditor* e = (MyEditor*) v;
    e->replace_dlg->hide();
}

void save_cb(void){
    if (filename[0] == '\0'){
        saveas_cb();
        return;
    }
    else save_file(filename);
}

void saveas_cb(void){
    char *newfile;
    newfile = fl_file_chooser("Save Files As?", "*", filename);
    if (newfile != NULL) save_file(newfile); 
}

int check_save(void){
    if (!changed) return 1;

    int r = fl_choice("The current file has not been saved\n" "Would you like to save it now? \n ", "Cancel", "Save", "Discard");
    if (r == 1){
        save_cb();
        return !changed;
    }
    return (r == 2) ? 1 : 0;
}


void load_file(char *newfile, int ipos){
    loading = 1;
    int insert = (ipos != -1);
    changed = insert;
    if (!insert) strcpy(filename, "");
    int r;
    if (!insert) r = textbuf->loadfile(newfile);
    else r = textbuf->insertfile(newfile, ipos);
    if (r)
        fl_alert("Error reading from file \'%s\':\n%s.", newfile, strerror(errno));
    else
        if (!insert) strcpy(filename, newfile);
    loading = 0;
    textbuf->call_modify_callbacks();
}

void save_file(char *newfile){
    if (textbuf->savefile(newfile)){
        fl_alert("Error writing to file \'%s\':\n%s.", newfile, strerror(errno));
    }
    else {
        strcpy(filename, newfile);
    }
    changed = 0;
    textbuf->call_modify_callbacks();
}

void set_title(Fl_Window *w){
    if (filename[0] == '\0') strcpy(title, "Untitled");
    else {
        char *slash;
        slash = strrchr(filename, '/');
#ifdef WIN32
        if (slash != NULL) strcpy(title, slash + 1);
        else strcpy(title, filename);
#endif
    }
    if (changed) strcat(title, " (modified)");

    w->label(title);
}

Fl_Window* new_view() {
    return new MyEditor(640, 400, "Text Editor");
}