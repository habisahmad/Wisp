#ifndef SYNTAX_HIGHLIGHTING_H
#define SYNTAX_HIGHLIGHTING_H
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>

inline Fl_Text_Display::Style_Table_Entry styletable[] = {
    { FL_BLACK,      FL_COURIER,        FL_NORMAL_SIZE }, // A - Plain
    { FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE }, // B - Line comment
    { FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE }, // C - Block comment
    { FL_BLUE,       FL_COURIER,        FL_NORMAL_SIZE }, // D - String
    { FL_DARK_RED,   FL_COURIER,        FL_NORMAL_SIZE }, // E - Directive
    { FL_DARK_RED,   FL_COURIER_BOLD,   FL_NORMAL_SIZE }, // F - Types
    { FL_BLUE,       FL_COURIER_BOLD,   FL_NORMAL_SIZE }  // G - Keywords
};
extern const char *code_keywords[];
extern const char *code_types[];
int compare_keywords(const void *, const void *);


void style_update(int pos, int nInserted, int nDeleted, int nRestyled, const char *deletedText, void *cbArg);

void style_parse(const char *text, char *style, int length);

void style_unfinished_cb(int, void*);

#endif