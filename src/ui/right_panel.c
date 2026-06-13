#include "raylib.h"
#include "raygui.h"
#include "right_panel.h"
#include <stdio.h>
#include "layout.h"
#include "ui/text.h"

extern Font g_font;

// Helper to map department enum to string
static const char* GetDeptName(DepartmentType dept) {
    switch (dept) {
        case DEPARTMENT_INTERNAL: return "내과";
        case DEPARTMENT_SURGERY: return "외과";
        case DEPARTMENT_ORTHOPEDICS: return "정형외과";
        default: return "알수없음";
    }
}

// Helper to map priority to short string
static const char* GetPriorityShort(EmergencyLevel lvl) {
    switch (lvl) {
        case EMERGENCY_LEVEL_1: return "P1";
        case EMERGENCY_LEVEL_2: return "P2";
        case EMERGENCY_LEVEL_3: return "P3";
        case EMERGENCY_LEVEL_4: return "P4";
        default: return "??";
    }
}

// Helper to get priority color
static Color GetPriorityColor(EmergencyLevel lvl) {
    switch (lvl) {
        case EMERGENCY_LEVEL_1: return COL_P1_TEXT;
        case EMERGENCY_LEVEL_2: return COL_P2_TEXT;
        case EMERGENCY_LEVEL_3: return COL_P3_TEXT;
        case EMERGENCY_LEVEL_4: return COL_P4_TEXT;
        default: return COL_TEXT;
    }
}

void DrawRightPanel(PatientSystem* system) {
    if (!system) return;

    int x = RIGHT_PANEL_X + 10;
    int w = RIGHT_PANEL_W - 20;
    int y = RIGHT_PANEL_Y + 12;

    // 패널 타이틀
    DrawKoreanText("환자 호출 & 병상", x, y, FONT_BODY, COL_TEXT);
    y += 28;

    // 호출 버튼 (대기 환자가 있는 경우에만 활성화 또는 클릭 시 작동)
    // raygui GuiButton 사용
    bool has_waiting = (system->normal_queue.size > 0 || system->emergency_queue.size > 0);
    int old_state = GuiGetState();
    if (!has_waiting) {
        GuiSetState(STATE_DISABLED);
    }
    
    if (GuiButton((Rectangle){(float)x, (float)y, (float)w, 38.0f}, "다음 환자 호출")) {
        patient_system_call_next(system);
    }
    GuiSetState(old_state);
    y += 50;

    // 병상 현황 구분선 및 타이틀
    DrawLine(RIGHT_PANEL_X + 5, y, RIGHT_PANEL_X + RIGHT_PANEL_W - 5, y, COL_BORDER);
    y += 12;
    DrawKoreanText("병상 배정 현황", x, y, FONT_SMALL, COL_TEXT_SEC);
    y += 24;

    // 6개 병상 그리기
    for (int i = 0; i < ER_BED_COUNT; i++) {
        Rectangle bed_rect = {(float)x, (float)y, (float)w, 68.0f};
        Patient* patient = system->beds[i];

        if (patient) {
            // 배정된 병상 배경
            DrawRectangleRounded(bed_rect, (float)BORDER_RADIUS / 16.0f, 4, COL_ROOM_OCCUPIED_BG);
            DrawRectangleRoundedLines(bed_rect, (float)BORDER_RADIUS / 16.0f, 4, COL_ROOM_OCCUPIED_TEXT);

            // 병상 정보
            char title_buf[32];
            snprintf(title_buf, sizeof(title_buf), "%d번 병상 [%s]", i + 1, GetPriorityShort(patient->emergency_level));
            DrawKoreanText(title_buf, x + 8, y + 6, FONT_SMALL, GetPriorityColor(patient->emergency_level));

            char info_buf[64];
            snprintf(info_buf, sizeof(info_buf), "%s(%d) / %s", patient->name, patient->age, GetDeptName(patient->department));
            DrawKoreanText(info_buf, x + 8, y + 26, FONT_SMALL, COL_TEXT);

            // 완료 버튼
            // raygui GuiButton을 병상 안에 작게 그린다.
            Rectangle fin_btn = {(float)(x + w - 58), (float)(y + 38), 50.0f, 22.0f};
            if (GuiButton(fin_btn, "완료")) {
                patient_system_finish_treatment(system, i);
            }
        } else {
            // 빈 병상 배경
            DrawRectangleRounded(bed_rect, (float)BORDER_RADIUS / 16.0f, 4, COL_ROOM_FREE_BG);
            DrawRectangleRoundedLines(bed_rect, (float)BORDER_RADIUS / 16.0f, 4, COL_BORDER);

            // 빈 병상 정보
            char title_buf[32];
            snprintf(title_buf, sizeof(title_buf), "%d번 병상 [비어있음]", i + 1);
            DrawKoreanText(title_buf, x + 8, y + 8, FONT_SMALL, COL_ROOM_FREE_TEXT);
            DrawKoreanText("대기 중...", x + 8, y + 32, FONT_SMALL, COL_TEXT_HINT);
        }

        y += 76;
    }
}

void RightPanel_HandleClick(PatientSystem* system, int mouse_x, int mouse_y) {
    if (!system) return;

    // 이 함수는 레이아웃 외곽의 raygui 클릭 감지에 실패할 수 있는
    // 커스텀 클릭 핸들링용(필요 시)으로 남겨둡니다.
    // raygui GuiButton 자체에 반환값이 있어서 그리기 도중 클릭이 처리됩니다.
}
