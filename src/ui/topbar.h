#ifndef ER_TOPBAR_H
#define ER_TOPBAR_H

#include "layout.h"
#include "raylib.h"

// 상단바에 표시할 데이터 구조체
typedef struct {
    int waiting_count;
    int treating_count;
    int total_processed;
} TopbarStats;

void DrawTopbar(TopbarStats stats);

#endif  // ER_TOPBAR_H