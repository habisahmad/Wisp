#include <editor_window.hpp>


Fl_Menu_Item menu[] = {
    { "&File", 0, 0, 0, Fl_SUBMENU },
        { "&New File", 0, (Fl_Callback *)new_cb },
        { "&Open File...", FL_CTRL + 'o', (Fl_Callback *)open_cb },
        { "&Insert File...", FL_CTRL + 'i', (Fl_Callback *)insert_cb, 0, FL_MENU_DIVIDER },
        { "&Save File", FL_CTRL + 's', (Fl_Callback *)save_cb },
        { "Save File &As...", FL_CTRL + FL_SHIFT + 's', (Fl_Callback *)saveas_cb, 0, FL_MENU_DIVIDER },
        { "New &View", FL_ALT + 'v', (Fl_Callback *)view_cb, 0 },
        { "&Close View", FL_CTRL + 'w', (Fl_Callback *)close_cb, 0, FL_MENU_DIVIDER },
        { "E&xit", FL_CTRL + 'q', (Fl_Callback *)quit_cb, 0 },
        { 0 },

    { "&Edit", 0, 0, 0, Fl_SUBMENU },
        { "&Undo", FL_CTRL + 'z', (Fl_Callback *)undo_cb, 0, FL_MENU_DIVIDER },
        { "Cu&t", FL_CTRL + 'x', (Fl_Callback *)cut_cb },
        { "&Copy", FL_CTRL + 'c', (Fl_Callback *)copy_cb },
        { "&Paste", FL_CTRL + 'v', (Fl_Callback *)paste_cb }, 
        { "&Delete", 0, (Fl_Callback *)delete_cb },
        { 0 },

    { "&Search", 0, 0, 0, Fl_SUBMENU },
        { "&Find...", FL_CTRL + 'f', (Fl_Callback *)find_cb },
        { "F&ind Again", FL_CTRL + 'g', find2_cb },
        { "&Replace", FL_CTRL + 'r', replace_cb },
        { "Re&place Again", FL_CTRL + 't', replace2_cb },
        { 0 },

    { 0 }
};

MyEditor::MyEditor(int w, int h, const char* t) : Fl_Double_Window(w, h, t) {
    // Copy the menu items to menu bar 
    Fl_Menu_Bar *m = new Fl_Menu_Bar(0, 0, 640, 30);
    m->copy(menu);

    editor = new Fl_Text_Editor(0, 30, 640, 370);
    editor->buffer(textbuf);

    // Keeps track of changes to a file
    textbuf->add_modify_callback(changed_cb, w);
    textbuf->call_modify_callbacks();

    // Define font
    editor->textfont(FL_COURIER);


    // Replace dialog
    Fl_Window *replace_dlg = new Fl_Window(300, 105, "Replace");
    Fl_Input *replace_find = new Fl_Input(70, 10, 200, 5, "Find:");
    Fl_Input *replace_with = new Fl_Input(70, 40, 200, 25, "Replace:");
    Fl_Button *replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    Fl_Button *replace_next = new Fl_Button(105, 70, 120, 25, "Replace Next");
    Fl_Button *replace_cancel = new Fl_Button(230, 60, 70, 25, "Cancel");
}



void changed_cb(int, int nInserted, int nDeleted,int, const char*, void* v) {
    // Change changed variable to 1
    if ((nInserted || nDeleted) && !loading) changed = 1;
    MyEditor *w = (MyEditor *)v;
    // set title to changed
    set_title(w);
    if (loading) w->editor->show_insert_position(); 
}

// Call kf_copy to copy to clipboard
void copy_cb(Fl_Widget*, void* v) {
    MyEditor* e = (MyEditor *)v;
    Fl_Text_Editor::kf_copy(0, e->editor);
}

void cut_cb(Fl_Widget*, void* v) {
    MyEditor* e = (myEditor *)v;
    Fl_Text_Editor::kf_cut(0, e->editor);
}

void delete_cb(Fl_Widget*, void* v) {
    textbuf->remove_selection();
}

void find_cb(Fl_Widget* w, void* v){
    MyEditor* e = (MyEditor*) v;
    // Copy User Input to val
    const char* val = fl_input("Search String:", e->search);
    // If user entered a string
    if (val != NULL){
        strcpy(e->search, val);
        find2_cb(w, v);
    }
}

void find2_cb(Fl_Widget* w, void* v) {
    MyEditor* e = (MyEditor*)v;

    if (e->search[0] == '\0'){
        // If string is blank get another
        find_cb(w, v);
        return;
    }
    int pos = e->editor->insert_position();
    int found = textbuf->search_forward(pos, e->search, &pos);
    if (found) {
        textbuf->select(pos, pos+strlen(e->search));
        e->editor->insert_position(pos+strlen(e->search));
        e->editor->show_insert_position();
    }
    else fl_alert("No occurences of \'%s'\ found", e->search);
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
    MyEditor* e = MyEditor* v;
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
