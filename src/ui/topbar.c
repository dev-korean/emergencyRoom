#include "topbar.h"

#include <stdio.h>
#include <time.h>

#include "raylib.h"

extern Font g_font;

// Local constants
#define TITLE_X 16
#define TITLE_Y 14
#define STAT_OFFSET_RIGHT 320
#define STAT_Y 12
#define STAT_SPACING 100
#define TIME_STR_SIZE 16
#define INFO_STR_SIZE 32
#define TIME_COLOR_ALPHA 220

void DrawTopbar(TopbarStats stats) {
    // 배경
    DrawRectangle(0, 0, WINDOW_WIDTH, TOPBAR_HEIGHT, COL_TOPBAR);

    // 왼쪽: 타이틀 아이콘 + 텍스트
    DrawTextEx(g_font, "응급실 환자 관리 시스템", (Vector2){(float)TITLE_X, (float)TITLE_Y},
               FONT_BODY, 2, COL_TEXT_WHITE);

    // 오른쪽: 통계 정보
    int stat_x = WINDOW_WIDTH - STAT_OFFSET_RIGHT;
    int stat_y = STAT_Y;
    int stat_spacing = STAT_SPACING;

    // 현재 시간 가져오기
    time_t time_val = time(NULL);
    struct tm* local_tm = localtime(&time_val);
    char time_str[TIME_STR_SIZE];
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", local_tm->tm_hour, local_tm->tm_min,
             local_tm->tm_sec);

    // 시간 표시
    DrawTextEx(g_font, time_str, (Vector2){(float)stat_x, (float)(stat_y + 2)}, FONT_SMALL, 2,
               (Color){COL_TEXT_WHITE.r, COL_TEXT_WHITE.g, COL_TEXT_WHITE.b, TIME_COLOR_ALPHA});

    // 대기 환자
    char waiting_str[INFO_STR_SIZE];
    snprintf(waiting_str, sizeof(waiting_str), "대기: %d", stats.waiting_count);
    DrawTextEx(g_font, waiting_str, (Vector2){(float)(stat_x + stat_spacing), (float)(stat_y + 2)},
               FONT_SMALL, 2,
               (Color){COL_TEXT_WHITE.r, COL_TEXT_WHITE.g, COL_TEXT_WHITE.b, TIME_COLOR_ALPHA});

    // 진료중 환자
    char treating_str[INFO_STR_SIZE];
    snprintf(treating_str, sizeof(treating_str), "진료중: %d", stats.treating_count);
    DrawTextEx(g_font, treating_str,
               (Vector2){(float)(stat_x + (stat_spacing * 2)), (float)(stat_y + 2)}, FONT_SMALL, 2,
               (Color){COL_TEXT_WHITE.r, COL_TEXT_WHITE.g, COL_TEXT_WHITE.b, TIME_COLOR_ALPHA});
}