#ifndef ER_LAYOUT_H
#define ER_LAYOUT_H

#include "raylib.h"

// ============================================================================
// 윈도우 설정
// ============================================================================
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define TARGET_FPS 60

// ============================================================================
// 레이아웃 크기
// ============================================================================// 레이아웃 크기
#define TOPBAR_HEIGHT 56
#define SIDEBAR_WIDTH 240
#define RIGHT_PANEL_W 240

// 계산된 영역
#define MAIN_AREA_X (SIDEBAR_WIDTH)
#define MAIN_AREA_Y (TOPBAR_HEIGHT)
#define MAIN_AREA_W (WINDOW_WIDTH - SIDEBAR_WIDTH - RIGHT_PANEL_W)
#define MAIN_AREA_H (WINDOW_HEIGHT - TOPBAR_HEIGHT)

#define RIGHT_PANEL_X (WINDOW_WIDTH - RIGHT_PANEL_W)
#define RIGHT_PANEL_Y (TOPBAR_HEIGHT)

// ============================================================================
// 색상 팔레트
// ============================================================================

// 헤더 및 주요 색상
#define COL_TOPBAR (Color){26, 115, 200, 255}   // #1A73C8
#define COL_PRIMARY (Color){26, 115, 200, 255}  // 주요 버튼

// 배경
#define COL_BG (Color){245, 247, 250, 255}          // #F5F7FA (전체 배경)
#define COL_SURFACE (Color){255, 255, 255, 255}     // 카드/패널 배경
#define COL_SIDEBAR_BG (Color){255, 255, 255, 255}  // 사이드바 배경
#define COL_HOVER (Color){232, 240, 253, 255}       // #E8F0FD (hover 상태)

// 테두리 및 선
#define COL_BORDER (Color){220, 223, 228, 255}       // 구분선
#define COL_BORDER_DARK (Color){200, 205, 213, 255}  // 강한 구분선

// 텍스트
#define COL_TEXT (Color){30, 30, 30, 255}           // 본문
#define COL_TEXT_SEC (Color){120, 120, 130, 255}    // 보조 텍스트
#define COL_TEXT_HINT (Color){160, 160, 170, 255}   // 힌트 텍스트
#define COL_TEXT_WHITE (Color){255, 255, 255, 255}  // 흰색 텍스트

// 우선순위 배지 (배경)
#define COL_P1_BG (Color){252, 232, 232, 255}  // #FCE8E8
#define COL_P1_TEXT (Color){192, 57, 43, 255}  // #C0392B

#define COL_P2_BG (Color){254, 243, 226, 255}   // #FEF3E2
#define COL_P2_TEXT (Color){183, 119, 13, 255}  // #B7770D

#define COL_P3_BG (Color){232, 240, 253, 255}   // #E8F0FD
#define COL_P3_TEXT (Color){26, 115, 200, 255}  // #1A73C8

#define COL_P4_BG (Color){234, 245, 232, 255}  // #EAF5E8
#define COL_P4_TEXT (Color){30, 126, 52, 255}  // #1E7E34

// 병상 상태
#define COL_ROOM_OCCUPIED_BG (Color){252, 232, 232, 255}
#define COL_ROOM_OCCUPIED_TEXT (Color){192, 57, 43, 255}

#define COL_ROOM_FREE_BG (Color){234, 245, 232, 255}
#define COL_ROOM_FREE_TEXT (Color){30, 126, 52, 255}

#define COL_ROOM_CLEANING_BG (Color){254, 243, 226, 255}
#define COL_ROOM_CLEANING_TEXT (Color){183, 119, 13, 255}

// 통계 카드 (상태별)
#define COL_STAT_CRITICAL (Color){192, 57, 43, 255}  // P1 빨강
#define COL_STAT_WARNING (Color){183, 119, 13, 255}  // P2 주황
#define COL_STAT_INFO (Color){26, 115, 200, 255}     // P3 파랑
#define COL_STAT_SUCCESS (Color){30, 126, 52, 255}   // P4 녹색

// ============================================================================
// 상수
// ============================================================================
#define PADDING_SMALL 8
#define PADDING_MID 12
#define PADDING_LARGE 16

#define BORDER_RADIUS 8
#define BORDER_RADIUS_CARD 12

#define LAYOUT_BORDER_WIDTH 1

// 폰트 크기
#define FONT_TITLE 25
#define FONT_HEADING 22
#define FONT_BODY 20
#define FONT_SMALL 18

#define HALF_DIV 2.0F

// 테이블
#define TABLE_ROW_HEIGHT 36
#define TABLE_HEADER_HEIGHT 40

#endif  // ER_LAYOUT_H