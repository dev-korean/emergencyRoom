#include "sidebar.h"

#include <stdio.h>
#include <stdlib.h>

#include "layout.h"
// string.h not needed here

#include "text.h"

// Local UI constants
#define ICON_SIZE 20
#define ITEM_H 44
#define ITEM_SPACING 6
#define BADGE_W 28
#define BADGE_H 22
#define BADGE_TEXT_SIZE 11
#define BADGE_LABEL_Y_OFFSET 6
#define BADGE_THRESHOLD 2
#define BADGE_DEFAULT 7
#define BADGE_STR_BUF 16
#define RADIUS_DIV_LOCAL 16.0F

Sidebar* Sidebar_Create(void) {
    Sidebar* sidebar = (Sidebar*)malloc(sizeof(Sidebar));
    sidebar->current_view = VIEW_DASHBOARD;

    // 메뉴 항목 초기화
    const char* labels[] = {"대시보드", "환자 등록", "대기 목록", "진료 현황", "처치 기록", "통계"};

    int badges[] = {BADGE_DEFAULT, 0, 2, 0, 0, 0};

    for (int i = 0; i < VIEW_COUNT; i++) {
        sidebar->items[i].view_id = (AppView)i;
        sidebar->items[i].label = labels[i];
        sidebar->items[i].badge_count = badges[i];
        sidebar->items[i].is_active = (i == VIEW_DASHBOARD) ? 1 : 0;
    }

    return sidebar;
}

void Sidebar_UpdateView(Sidebar* sidebar, AppView new_view) {
    // 이전 활성 항목 비활성화
    for (int i = 0; i < VIEW_COUNT; i++) {
        sidebar->items[i].is_active = 0;
    }
    // 새 항목 활성화
    sidebar->items[new_view].is_active = 1;
    sidebar->current_view = new_view;
}

void Sidebar_SetBadge(Sidebar* sidebar, AppView view_id, int count) {
    if (view_id >= 0 && view_id < VIEW_COUNT) {
        sidebar->items[view_id].badge_count = count;
    }
}

// 메뉴 아이템 하나 그리기
static void DrawNavItem(NavItem* item, Rectangle rect, int is_active) {
    int pos_x = (int)rect.x;
    int pos_y = (int)rect.y;
    int width = (int)rect.width;
    int height = (int)rect.height;
    // 배경
    if (is_active) {
        // 활성 상태: 밝은 파란색
        DrawRectangle(pos_x, pos_y, width, height, COL_HOVER);
        DrawRectangleLines(pos_x, pos_y, width, height, COL_PRIMARY);
    } else {
        // 기본 상태: 투명
        DrawRectangleLines(pos_x, pos_y, width, height, COL_BORDER);
    }

    // 아이콘 자리 (간단하게 동그라미)
    int icon_x = pos_x + PADDING_MID;
    int icon_y = pos_y + ((height - ICON_SIZE) / 2);
    int center_x = icon_x + (ICON_SIZE / 2);
    int center_y = icon_y + (ICON_SIZE / 2);
    float radius = (float)ICON_SIZE / HALF_DIV;
    DrawCircle(center_x, center_y, radius, is_active ? COL_PRIMARY : COL_TEXT_SEC);

    // 라벨 텍스트
    int text_x = icon_x + ICON_SIZE + PADDING_MID;
    int text_y = pos_y + ((height - FONT_SMALL) / 2);
    Color text_color = is_active ? COL_PRIMARY : COL_TEXT;
    DrawKoreanText(item->label, text_x, text_y, FONT_SMALL, text_color);

    // 배지 (알림 수)
    if (item->badge_count > 0) {
        char badge_str[BADGE_STR_BUF];
        snprintf(badge_str, sizeof(badge_str), "%d", item->badge_count);

        int badge_x = pos_x + width - PADDING_MID - BADGE_W;
        int badge_y = pos_y + ((height - BADGE_H) / 2);

        // 배지 배경
        Color badge_bg;
        if (is_active) {
            badge_bg = COL_PRIMARY;
        } else if (item->badge_count > BADGE_THRESHOLD) {
            badge_bg = COL_P1_BG;
        } else {
            badge_bg = COL_P2_BG;
        }

        Color badge_text;
        if (is_active) {
            badge_text = COL_TEXT_WHITE;
        } else if (item->badge_count > BADGE_THRESHOLD) {
            badge_text = COL_P1_TEXT;
        } else {
            badge_text = COL_P2_TEXT;
        }

        DrawRectangleRounded(
            (Rectangle){(float)badge_x, (float)badge_y, (float)BADGE_W, (float)BADGE_H},
            (float)BORDER_RADIUS / RADIUS_DIV_LOCAL, 2, badge_bg);

        int num_w = MeasureText(badge_str, BADGE_TEXT_SIZE);
        int num_x = badge_x + ((BADGE_W - num_w) / 2);
        DrawText(badge_str, num_x, badge_y + BADGE_LABEL_Y_OFFSET, BADGE_TEXT_SIZE, badge_text);
    }
}

void DrawSidebar(Sidebar* sidebar) {
    // 배경
    DrawRectangle(0, TOPBAR_HEIGHT, SIDEBAR_WIDTH, WINDOW_HEIGHT - TOPBAR_HEIGHT, COL_SIDEBAR_BG);

    // 구분선
    DrawRectangleLines(0, TOPBAR_HEIGHT, SIDEBAR_WIDTH, WINDOW_HEIGHT - TOPBAR_HEIGHT, COL_BORDER);

    // "메뉴" 레이블
    int menu_label_y = TOPBAR_HEIGHT + PADDING_MID;
    DrawKoreanText("메뉴", PADDING_MID, menu_label_y, FONT_SMALL, COL_TEXT_SEC);

    // 네비게이션 항목들
    int item_y = menu_label_y + FONT_SMALL + PADDING_MID;
    for (int i = 0; i < VIEW_COUNT; i++) {
        Rectangle rect = {(float)PADDING_SMALL, (float)item_y,
                          (float)(SIDEBAR_WIDTH - (PADDING_SMALL * 2)), (float)ITEM_H};
        DrawNavItem(&sidebar->items[i], rect, sidebar->items[i].is_active);
        item_y += ITEM_H + ITEM_SPACING;
    }
}

// 클릭 감지 및 뷰 변경
AppView Sidebar_HandleClick(Sidebar* sidebar, int mouse_x, int mouse_y) {
    // 사이드바 범위 체크
    if (mouse_x < 0 || mouse_x > SIDEBAR_WIDTH || mouse_y < TOPBAR_HEIGHT) {
        return sidebar->current_view;
    }

    // 메뉴 레이블 아래부터 시작
    int item_y = TOPBAR_HEIGHT + PADDING_MID + FONT_SMALL + PADDING_MID;
    int item_h = ITEM_H;
    int item_spacing = ITEM_SPACING;

    // 각 항목 영역 체크
    for (int i = 0; i < VIEW_COUNT; i++) {
        int item_x = PADDING_SMALL;
        int item_w = SIDEBAR_WIDTH - (PADDING_SMALL * 2);

        if (mouse_x > item_x && mouse_x < item_x + item_w && mouse_y > item_y &&
            mouse_y < item_y + item_h) {
            // 클릭된 메뉴 항목
            Sidebar_UpdateView(sidebar, (AppView)i);
            return (AppView)i;
        }

        item_y += item_h + item_spacing;
    }

    return sidebar->current_view;
}

void Sidebar_Destroy(Sidebar* sidebar) {
    if (sidebar) {
        free(sidebar);
    }
}