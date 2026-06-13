#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include <stdlib.h>

#include "raygui.h"
#include "systems/patient_system.h"
#include "ui/layout.h"
#include "ui/main_view.h"
#include "ui/right_panel.h"
#include "ui/sidebar.h"
#include "ui/text.h"
#include "ui/topbar.h"

// UI에서 실제로 표시하는 문자열들을 폰트 아틀라스에 포함시킨다.
// 상용 한글 2350자를 포함하도록 범위를 지정하는 것이 좋지만,
// 여기서는 필요한 글자들과 자주 쓰이는 글자들을 대폭 추가한다.
static const char* UI_FONT_GLYPHS =
    "응급실환자관리시스템대기진료중총위험일반준긴급완료알수없음"
    "우선순위목록오늘처리평균흉통호흡곤란두부외상의식저하복통구토발목골절고열오한"
    "김민준이서윤박현우최지아정우진내과외과정형외과검색조건맞는취소번성명시간분초"
    "배정지않았습우측다음호출현황비어있대기중통계분석누적비율분포"
    "신규접수이름나이세건"
    "보드등치및병박최처가니다을해를요별수되돌마적"
    "가각간갇갈갉갊갋감갑값갓갔강갖갗같갚갛개객갠갤갬갭갯갰갱갸갹갼걀걋걍걔걘걜거걱건걷걸걺검겁것겄"
    "헝겊겋게겐겔겜겝겟겠겡겨격견곈결겸겹겻겼경곁계곈곌곕곗고곡곤곧골곪곬곯곰곱곳공곶과곽관괄괆괌괍"
    "괏광쾌괜퇘괴괵괸괼굄굅굇굉교굔굘굡굣구국군굳굴굵굶굻굼굽굿궁궂궈궉권궐궤궤귀귁귄귈귐귑귓규균귤"
    "그극근귿글긁금급긋긍긔기긱긴긷길김깁깃깅깊기";

// 전역 폰트 변수
Font g_font = {0};

int main(void) {
    // 윈도우 초기화
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "응급실 환자 관리 시스템");
    SetTargetFPS(TARGET_FPS);

    // 한글 유니코드 범위: 0xAC00 ~ 0xD7A3 (11,172자)
    // ASCII 범위: 32 ~ 126 (95자)
    int hangul_range_count = 0xD7A3 - 0xAC00 + 1;
    int ascii_range_count = 95;

    // UI_FONT_GLYPHS 문자열에 포함된 글자들도 추가로 추출 (중복 상관없음)
    int extra_glyphs_count = 0;
    int* extra_glyphs = LoadCodepoints(UI_FONT_GLYPHS, &extra_glyphs_count);

    int total_glyph_count = ascii_range_count + hangul_range_count + extra_glyphs_count;
    int* all_glyphs = (int*)malloc(sizeof(int) * total_glyph_count);

    int idx = 0;
    // 1. ASCII 채우기
    for (int i = 0; i < ascii_range_count; i++) {
        all_glyphs[idx++] = 32 + i;
    }
    // 2. 한글 전체 범위 채우기 (가 ~ 힣)
    for (int i = 0; i < hangul_range_count; i++) {
        all_glyphs[idx++] = 0xAC00 + i;
    }
    // 3. UI_FONT_GLYPHS 특수 문자 등 채우기
    for (int i = 0; i < extra_glyphs_count; i++) {
        all_glyphs[idx++] = extra_glyphs[i];
    }

    // 폰트 로드 (전체 한글 범위를 포함하여 로드)
    g_font = LoadFontEx("assets/fonts/malgun.ttf", FONT_TITLE, all_glyphs, total_glyph_count);

    free(all_glyphs);
    if (extra_glyphs) UnloadCodepoints(extra_glyphs);

    if (g_font.texture.id > 0 && g_font.glyphCount > 0) {
        SetTextureFilter(g_font.texture, TEXTURE_FILTER_BILINEAR);
        GuiSetFont(g_font);
    }

    // 폰트 로드 실패 시 대체
    if (g_font.glyphCount == 0) {
        TraceLog(LOG_WARNING, "폰트 로드 실패, 기본 폰트 사용");
        g_font = GetFontDefault();
        GuiSetFont(g_font);
    }

    // raygui 스타일 설정
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_BODY);

    // 환자 관리 시스템 초기화
    PatientSystem sys;
    patient_system_init(&sys);

    // 테스트용 환자 등록
    patient_system_register_emergency(
        &sys, patient_create(1001, "김민준", 62, "흉통, 호흡곤란", EMERGENCY_LEVEL_1,
                             DEPARTMENT_INTERNAL, PATIENT_WAITING));
    patient_system_register_emergency(
        &sys, patient_create(1002, "이서윤", 45, "두부외상, 의식저하", EMERGENCY_LEVEL_1,
                             DEPARTMENT_SURGERY, PATIENT_WAITING));
    patient_system_register_emergency(
        &sys, patient_create(1003, "박현우", 31, "복통, 구토", EMERGENCY_LEVEL_2,
                             DEPARTMENT_INTERNAL, PATIENT_WAITING));
    patient_system_register_emergency(
        &sys, patient_create(1004, "최지아", 27, "발목 골절", EMERGENCY_LEVEL_3,
                             DEPARTMENT_ORTHOPEDICS, PATIENT_WAITING));
    patient_system_register_normal(
        &sys, patient_create(1005, "정우진", 55, "고열, 오한", EMERGENCY_LEVEL_4,
                             DEPARTMENT_INTERNAL, PATIENT_WAITING));

    // 시작 시 병상에 환자 일부 배정 (2명 호출)
    patient_system_call_next(&sys);
    patient_system_call_next(&sys);

    // 사이드바 생성
    Sidebar* sidebar = Sidebar_Create();

    // 메인 루프
    while (!WindowShouldClose()) {
        // 클릭 감지 및 사이드바 선택 변경
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Sidebar_HandleClick(sidebar, GetMouseX(), GetMouseY());
        }

        // 대기 수, 진료중 수, 완료 수 계산
        int waiting_count = (int)(sys.normal_queue.size + sys.emergency_queue.size);
        int treating_count = 0;
        for (int i = 0; i < ER_BED_COUNT; i++) {
            if (sys.beds[i] != NULL) {
                treating_count++;
            }
        }
        int processed_count = (int)sys.history.size;

        // 사이드바 알림 배지 카운트 실시간 업데이트
        Sidebar_SetBadge(sidebar, VIEW_WAITING_LIST, waiting_count);
        Sidebar_SetBadge(sidebar, VIEW_TREATING, treating_count);
        Sidebar_SetBadge(sidebar, VIEW_HISTORY, processed_count);

        // 상단바 통계 실시간 생성
        TopbarStats topbar_stats = {.waiting_count = waiting_count,
                                    .treating_count = treating_count,
                                    .total_processed = processed_count};

        BeginDrawing();
        ClearBackground(COL_BG);

        // 상단바 그리기
        DrawTopbar(topbar_stats);

        // 사이드바 그리기
        DrawSidebar(sidebar);

        // 메인 뷰 그리기 (현재 선택된 사이드바 메뉴에 따라)
        switch (sidebar->current_view) {
            case VIEW_DASHBOARD:
                DrawDashboardView(&sys);
                break;
            case VIEW_REGISTER:
                DrawRegisterView(&sys);
                break;
            case VIEW_WAITING_LIST:
                DrawWaitingListView(&sys);
                break;
            case VIEW_TREATING:
                DrawTreatingView(&sys);
                break;
            case VIEW_HISTORY:
                DrawHistoryView(&sys);
                break;
            case VIEW_STATS:
                DrawStatsView(&sys);
                break;
            default:
                DrawRectangle(MAIN_AREA_X, MAIN_AREA_Y, MAIN_AREA_W, MAIN_AREA_H, COL_BG);
                break;
        }

        // 우측 패널 그리기 (병상 현황 및 긴급 호출)
        DrawRectangle(RIGHT_PANEL_X, TOPBAR_HEIGHT, RIGHT_PANEL_W, WINDOW_HEIGHT - TOPBAR_HEIGHT,
                      COL_SURFACE);
        DrawRectangleLines(RIGHT_PANEL_X - LAYOUT_BORDER_WIDTH, TOPBAR_HEIGHT,
                           RIGHT_PANEL_W + LAYOUT_BORDER_WIDTH, WINDOW_HEIGHT - TOPBAR_HEIGHT,
                           COL_BORDER);
        DrawRightPanel(&sys);

        EndDrawing();
    }

    // 메모리 해제
    Sidebar_Destroy(sidebar);
    patient_system_clear(&sys);
    UnloadFont(g_font);
    CloseWindow();

    return 0;
}
