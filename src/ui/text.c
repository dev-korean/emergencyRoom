#include "text.h"

#include "raylib.h"

extern Font g_font;

#define TEXT_SPACING 2.0F

void DrawKoreanText(const char* text, int pos_x, int pos_y, int fontSize, Color color) {
    DrawTextEx(g_font, text, (Vector2){(float)pos_x, (float)pos_y}, (float)fontSize, TEXT_SPACING,
               color);
}
