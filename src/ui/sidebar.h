#ifndef ER_SIDEBAR_H
#define ER_SIDEBAR_H

#include <stddef.h>

// 현재 보여줄 뷰 상태
typedef enum {
    VIEW_DASHBOARD = 0,
    VIEW_REGISTER = 1,
    VIEW_WAITING_LIST = 2,
    VIEW_TREATING = 3,
    VIEW_HISTORY = 4,
    VIEW_STATS = 5,
    VIEW_COUNT = 6
} AppView;

// 네비게이션 메뉴 아이템
typedef struct {
    AppView view_id;
    const char* label;
    int badge_count;  // 0이면 배지 안 보임
    int is_active;
} NavItem;

// 사이드바 상태
typedef struct {
    AppView current_view;
    NavItem items[VIEW_COUNT];
} Sidebar;

// 함수
Sidebar* Sidebar_Create(void);
void Sidebar_UpdateView(Sidebar* sidebar, AppView new_view);
void Sidebar_SetBadge(Sidebar* sidebar, AppView view_id, int count);
void DrawSidebar(Sidebar* sidebar);
AppView Sidebar_HandleClick(Sidebar* sidebar, int mouse_x, int mouse_y);
void Sidebar_Destroy(Sidebar* sidebar);

#endif  // ER_SIDEBAR_H