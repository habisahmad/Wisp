#include <cstring>
#include <cstdlib>
#include <cctype>      
#include "syntax_highlighting.hpp"
#include "editor_window.hpp"



void style_unfinished_cb(int, void*) {
    // Not needed for basic highlighting
}

int compare_keywords(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}


void style_update(int pos, int nInserted, int nDeleted, int, const char*, void* cbArg) {
    int start, end;
    char last, *style, *text;

    if (nInserted == 0 && nDeleted == 0){
        stylebuf->unselect();
        return;
    }
    if (nInserted > 0){
        style = new char[nInserted + 1];
        memset(style, 'A', nInserted);
        style[nInserted] = '\0';

        stylebuf->replace(pos, pos + nDeleted, style);
        delete[] style;
    } else {
        stylebuf->remove(pos, pos+nDeleted);
    }

    start = textbuf->line_start(pos);
    end = textbuf->line_end(pos +nInserted-nDeleted);
    text = textbuf->text_range(start, end);
    style = stylebuf->text_range(start, end);
    last = style[end - start - 1];

    style_parse(text, style, end - start);
    stylebuf->replace(start, end, style);
    ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);

    if (last != style[end - start -1]){
        free(text);
        free(style);

        end = textbuf->length();
        text = textbuf->text_range(start, end);
        style = textbuf->text_range(start, end);

        style_parse(text, style, end - start);

        stylebuf->replace(start, end, style);
        ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);
    }
    free(text);
    free(style);
}

void style_parse(const char *text, char *style, int length) {
    const char* keywords[] = { "if", "else", "while", "return", "for", "switch", "case", "break", "continue" };
    const char* types[] = { "int", "char", "void", "float", "double", "bool" };

    int i = 0;
    while (i < length) {
        if (strncmp(&text[i], "//", 2) == 0) {
            while (i < length && text[i] != '\n') {
                style[i++] = 'B'; // Line comment
            }
        } else if (text[i] == '\"') {
            style[i++] = 'D'; // Start string
            while (i < length && text[i] != '\"') {
                style[i++] = 'D';
            }
            if (i < length) style[i++] = 'D'; // End quote
        } else if (i == 0 && text[i] == '#') {
            while (i < length && text[i] != '\n') {
                style[i++] = 'E'; // Directive
            }
        } else if (isalpha(text[i])) {
            int start = i;
            while (i < length && isalnum(text[i])) i++;

            int word_len = i - start;
            char word[32];
            if (word_len < (int)sizeof(word)) {
                strncpy(word, &text[start], word_len);
                word[word_len] = '\0';

                bool matched = false;
                for (const char* kw : keywords) {
                    if (strcmp(kw, word) == 0) {
                        memset(&style[start], 'G', word_len); // Keyword
                        matched = true;
                        break;
                    }
                }
                if (!matched) {
                    for (const char* type : types) {
                        if (strcmp(type, word) == 0) {
                            memset(&style[start], 'F', word_len); // Type
                            break;
                        }
                    }
                }
            }
        } else {
            style[i++] = 'A'; // Default
        }
    }
}

