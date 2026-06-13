#include "main_view.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "layout.h"
#include "raygui.h"
#include "raylib.h"
#include "ui/text.h"

extern Font g_font;

#define ROW_ALT_COLOR ((Color){250, 250, 250, 255})

// Helper to calculate wait time description
static void GetWaitTimeDesc(Patient* p, char* buf, size_t buf_len) {
    if (!p) {
        snprintf(buf, buf_len, "-");
        return;
    }
    int seconds = (int)(time(NULL) - p->arrival_time);
    if (seconds < 60) {
        snprintf(buf, buf_len, "%d초", seconds);
    } else {
        snprintf(buf, buf_len, "%d분", seconds / 60);
    }
}

// Helper to get Korean string for department
static const char* GetDeptName(DepartmentType dept) {
    switch (dept) {
        case DEPARTMENT_INTERNAL:
            return "내과";
        case DEPARTMENT_SURGERY:
            return "외과";
        case DEPARTMENT_ORTHOPEDICS:
            return "정형외과";
        default:
            return "알수없음";
    }
}

// Helper to get priority label
static const char* GetPriorityLabel(EmergencyLevel lvl) {
    switch (lvl) {
        case EMERGENCY_LEVEL_1:
            return "P1 위험";
        case EMERGENCY_LEVEL_2:
            return "P2 긴급";
        case EMERGENCY_LEVEL_3:
            return "P3 준긴급";
        case EMERGENCY_LEVEL_4:
            return "P4 일반";
        default:
            return "알수없음";
    }
}

// Helper to get priority colors
static Color GetPriorityBgColor(EmergencyLevel lvl) {
    switch (lvl) {
        case EMERGENCY_LEVEL_1:
            return COL_P1_BG;
        case EMERGENCY_LEVEL_2:
            return COL_P2_BG;
        case EMERGENCY_LEVEL_3:
            return COL_P3_BG;
        case EMERGENCY_LEVEL_4:
            return COL_P4_BG;
        default:
            return COL_BG;
    }
}

static Color GetPriorityTextColor(EmergencyLevel lvl) {
    switch (lvl) {
        case EMERGENCY_LEVEL_1:
            return COL_P1_TEXT;
        case EMERGENCY_LEVEL_2:
            return COL_P2_TEXT;
        case EMERGENCY_LEVEL_3:
            return COL_P3_TEXT;
        case EMERGENCY_LEVEL_4:
            return COL_P4_TEXT;
        default:
            return COL_TEXT;
    }
}

// Collect all waiting patients in order
static int GetWaitingPatients(PatientSystem* system, Patient** out_patients, int max_count) {
    int count = 0;

    // 1. Emergency Queue
    PriorityQueueNode* eq_node = system->emergency_queue.front;
    while (eq_node && count < max_count) {
        out_patients[count++] = (Patient*)eq_node->data;
        eq_node = eq_node->next;
    }

    // 2. Normal Queue
    QueueNode* nq_node = system->normal_queue.front;
    while (nq_node && count < max_count) {
        out_patients[count++] = (Patient*)nq_node->data;
        nq_node = nq_node->next;
    }

    return count;
}

// 1. Dashboard View
void DrawDashboardView(PatientSystem* system) {
    DrawRectangle(MAIN_AREA_X, MAIN_AREA_Y, MAIN_AREA_W, MAIN_AREA_H, COL_BG);

    // Calculate stats
    int total_waiting = (int)(system->normal_queue.size + system->emergency_queue.size);

    int waiting_p1 = 0;
    int total_wait_sec = 0;
    ListNode* curr = system->patients.head;
    while (curr) {
        Patient* p = (Patient*)curr->data;
        if (p && p->status == PATIENT_WAITING) {
            if (p->emergency_level == EMERGENCY_LEVEL_1) {
                waiting_p1++;
            }
            total_wait_sec += (int)(time(NULL) - p->arrival_time);
        }
        curr = curr->next;
    }

    int avg_wait_min = 0;
    if (total_waiting > 0) {
        avg_wait_min = (total_wait_sec / total_waiting) / 60;
    }

    int total_processed = (int)system->history.size;

    // Draw Stat Cards
    int stat_y = MAIN_AREA_Y + PADDING_MID;
    int stat_h = 100;
    int stat_w = (MAIN_AREA_W - (PADDING_MID * 5)) / 4;
    int stat_x = MAIN_AREA_X + PADDING_MID;

    DrawStatCard(stat_x, stat_y, stat_w, stat_h, "총 대기 환자", total_waiting, COL_TEXT);
    stat_x += stat_w + PADDING_MID;

    DrawStatCard(stat_x, stat_y, stat_w, stat_h, "긴급 환자 (P1)", waiting_p1, COL_STAT_CRITICAL);
    stat_x += stat_w + PADDING_MID;

    DrawStatCard(stat_x, stat_y, stat_w, stat_h, "평균 대기 시간", avg_wait_min, COL_STAT_WARNING);
    stat_x += stat_w + PADDING_MID;

    DrawStatCard(stat_x, stat_y, stat_w, stat_h, "오늘 완료 건수", total_processed,
                 COL_STAT_SUCCESS);

    // Patients Table Area
    int table_y = stat_y + stat_h + (PADDING_MID * 2);
    int table_h = MAIN_AREA_H - (table_y - MAIN_AREA_Y) - PADDING_MID;

    DrawTextEx(g_font, "우선순위 대기 목록",
               (Vector2){(float)(MAIN_AREA_X + PADDING_MID), (float)(table_y - 32)}, FONT_BODY, 2,
               COL_TEXT);

    // Draw Table Box
    int box_x = MAIN_AREA_X + PADDING_MID;
    int box_w = MAIN_AREA_W - (PADDING_MID * 2);

    DrawRectangleRounded((Rectangle){(float)box_x, (float)table_y, (float)box_w, (float)table_h},
                         0.5f, 4, COL_SURFACE);
    DrawRectangleRoundedLines(
        (Rectangle){(float)box_x, (float)table_y, (float)box_w, (float)table_h}, 0.5f, 4,
        COL_BORDER);

    // Draw Header
    DrawRectangle(box_x + 1, table_y + 1, box_w - 2, TABLE_HEADER_HEIGHT, COL_HOVER);

    const char* headers[] = {"순번", "환자명 (나이)", "증상", "우선순위", "대기시간", "진료과"};
    int widths[] = {60, 160, 240, 110, 110, 100};
    int header_y = table_y + (TABLE_HEADER_HEIGHT - FONT_SMALL) / 2;
    int cur_x = box_x + PADDING_MID;

    for (int i = 0; i < 6; i++) {
        DrawKoreanText(headers[i], cur_x, header_y, FONT_SMALL, COL_TEXT_SEC);
        cur_x += widths[i];
    }

    // Collect and Render Patients
    Patient* waiting_list[100];
    int waiting_count = GetWaitingPatients(system, waiting_list, 100);

    int row_y = table_y + TABLE_HEADER_HEIGHT;
    int max_rows = (table_h - TABLE_HEADER_HEIGHT) / TABLE_ROW_HEIGHT;

    if (waiting_count == 0) {
        DrawKoreanText("현재 대기 중인 환자가 없습니다.", box_x + 30, row_y + 20, FONT_BODY,
                       COL_TEXT_HINT);
    } else {
        for (int i = 0; i < waiting_count && i < max_rows; i++) {
            if (i % 2 == 0) {
                DrawRectangle(box_x + 1, row_y, box_w - 2, TABLE_ROW_HEIGHT, ROW_ALT_COLOR);
            }

            Patient* pat = waiting_list[i];
            int text_y = row_y + (TABLE_ROW_HEIGHT - FONT_BODY) / 2;
            int cell_x = box_x + PADDING_MID;

            // 1. Rank
            char rank_str[8];
            snprintf(rank_str, sizeof(rank_str), "%d", i + 1);
            DrawKoreanText(rank_str, cell_x, text_y, FONT_BODY, COL_TEXT);
            cell_x += widths[0];

            // 2. Name (Age)
            char name_age[64];
            snprintf(name_age, sizeof(name_age), "%s (%d세)", pat->name, pat->age);
            DrawKoreanText(name_age, cell_x, text_y, FONT_BODY, COL_TEXT);
            cell_x += widths[1];

            // 3. Symptom
            char symptom_trunc[32];
            strncpy(symptom_trunc, pat->symptom, 20);
            symptom_trunc[20] = '\0';
            if (strlen(pat->symptom) > 20) {
                strcat(symptom_trunc, "...");
            }
            DrawKoreanText(symptom_trunc, cell_x, text_y, FONT_BODY, COL_TEXT_SEC);
            cell_x += widths[2];

            // 4. Priority Badge
            Color bg_col = GetPriorityBgColor(pat->emergency_level);
            Color txt_col = GetPriorityTextColor(pat->emergency_level);
            Rectangle badge = {(float)cell_x, (float)(row_y + 6), 90.0f, 24.0f};
            DrawRectangleRounded(badge, 0.3f, 4, bg_col);
            DrawRectangleRoundedLines(badge, 0.3f, 4, txt_col);
            int lbl_w = (int)MeasureTextEx(g_font, GetPriorityLabel(pat->emergency_level),
                                           (float)FONT_SMALL, 2.0f)
                            .x;
            DrawKoreanText(GetPriorityLabel(pat->emergency_level),
                           (int)(badge.x + (badge.width - lbl_w) / 2), (int)(badge.y + 3),
                           FONT_SMALL, txt_col);
            cell_x += widths[3];

            // 5. Wait Time
            char wait_desc[32];
            GetWaitTimeDesc(pat, wait_desc, sizeof(wait_desc));
            DrawKoreanText(wait_desc, cell_x, text_y, FONT_BODY, COL_TEXT);
            cell_x += widths[4];

            // 6. Department
            DrawKoreanText(GetDeptName(pat->department), cell_x, text_y, FONT_BODY, COL_TEXT);

            row_y += TABLE_ROW_HEIGHT;
        }
    }
}

void DrawStatCard(int pos_x, int pos_y, int width, int height, const char* label, int value,
                  Color value_color) {
    DrawRectangleRounded((Rectangle){(float)pos_x, (float)pos_y, (float)width, (float)height}, 0.1f,
                         4, COL_SURFACE);
    DrawRectangleRoundedLines((Rectangle){(float)pos_x, (float)pos_y, (float)width, (float)height},
                              0.1f, 4, COL_BORDER);

    // Label
    DrawTextEx(g_font, label,
               (Vector2){(float)(pos_x + PADDING_MID), (float)(pos_y + PADDING_SMALL)}, FONT_SMALL,
               2, COL_TEXT_SEC);

    // Value
    char val_str[32];
    if (strcmp(label, "평균 대기 시간") == 0) {
        snprintf(val_str, sizeof(val_str), "%d분", value);
    } else {
        snprintf(val_str, sizeof(val_str), "%d명", value);
    }
    int text_width = (int)MeasureTextEx(g_font, val_str, (float)FONT_TITLE, 2).x;
    int centered_x = pos_x + ((width - text_width) / 2);
    DrawTextEx(g_font, val_str, (Vector2){(float)centered_x, (float)(pos_y + 40)}, FONT_TITLE, 2,
               value_color);
}

// 2. Patient Registration View
void DrawRegisterView(PatientSystem* system) {
    DrawRectangle(MAIN_AREA_X, MAIN_AREA_Y, MAIN_AREA_W, MAIN_AREA_H, COL_BG);

    // Form inputs state
    static char reg_name[64] = "";
    static bool reg_name_edit = false;
    static int reg_age = 45;
    static char reg_symptom[128] = "";
    static bool reg_symptom_edit = false;
    static int reg_level_idx = 3;  // General P4 by default
    static int reg_dept_idx = 0;   // Internal by default
    static int id_counter = 1001;

    int panel_w = 600;
    int panel_h = 560;
    int panel_x = MAIN_AREA_X + (MAIN_AREA_W - panel_w) / 2;
    int panel_y = MAIN_AREA_Y + (MAIN_AREA_H - panel_h) / 2;

    // Outer Panel
    DrawRectangleRounded(
        (Rectangle){(float)panel_x, (float)panel_y, (float)panel_w, (float)panel_h}, 0.05f, 4,
        COL_SURFACE);
    DrawRectangleRoundedLines(
        (Rectangle){(float)panel_x, (float)panel_y, (float)panel_w, (float)panel_h}, 0.05f, 4,
        COL_BORDER);

    // Title
    DrawKoreanText("신규 환자 접수", panel_x + PADDING_LARGE, panel_y + PADDING_LARGE, FONT_HEADING,
                   COL_PRIMARY);
    DrawLine(panel_x + PADDING_LARGE, panel_y + 50, panel_x + panel_w - PADDING_LARGE, panel_y + 50,
             COL_BORDER);

    int cur_y = panel_y + 70;
    int label_x = panel_x + 30;
    int input_x = panel_x + 160;
    int input_w = 400;

    // 1. Patient Name
    DrawKoreanText("환자 성명", label_x, cur_y + 6, FONT_BODY, COL_TEXT);
    if (GuiTextBox((Rectangle){(float)input_x, (float)cur_y, (float)input_w, 36.0f}, reg_name, 64,
                   reg_name_edit)) {
        reg_name_edit = !reg_name_edit;
    }
    cur_y += 54;

    // 2. Patient Age
    DrawKoreanText("환자 나이", label_x, cur_y + 6, FONT_BODY, COL_TEXT);
    // Minus Button
    if (GuiButton((Rectangle){(float)input_x, (float)cur_y, 44.0f, 36.0f}, "-")) {
        if (reg_age > 0) reg_age--;
    }
    // Age Text
    char age_buf[16];
    snprintf(age_buf, sizeof(age_buf), "%d 세", reg_age);
    DrawRectangle(input_x + 50, cur_y, input_w - 100, 36, COL_BG);
    DrawRectangleLines(input_x + 50, cur_y, input_w - 100, 36, COL_BORDER);
    int age_w = (int)MeasureTextEx(g_font, age_buf, (float)FONT_BODY, 2.0f).x;
    DrawKoreanText(age_buf, input_x + 50 + (input_w - 100 - age_w) / 2, cur_y + 6, FONT_BODY,
                   COL_TEXT);
    // Plus Button
    if (GuiButton((Rectangle){(float)(input_x + input_w - 44), (float)cur_y, 44.0f, 36.0f}, "+")) {
        if (reg_age < 150) reg_age++;
    }
    cur_y += 54;

    // 3. Symptoms
    DrawKoreanText("주요 증상", label_x, cur_y + 6, FONT_BODY, COL_TEXT);
    if (GuiTextBox((Rectangle){(float)input_x, (float)cur_y, (float)input_w, 36.0f}, reg_symptom,
                   128, reg_symptom_edit)) {
        reg_symptom_edit = !reg_symptom_edit;
    }

    // Quick Symptom Suggestions
    int sug_y = cur_y + 42;
    const char* suggestions[] = {"흉통", "호흡곤란", "두부외상", "복통", "골절", "고열"};
    int sug_x = input_x;
    for (int i = 0; i < 6; i++) {
        if (GuiButton((Rectangle){(float)sug_x, (float)sug_y, 55.0f, 22.0f}, suggestions[i])) {
            strncpy(reg_symptom, suggestions[i], sizeof(reg_symptom) - 1);
            reg_symptom[sizeof(reg_symptom) - 1] = '\0';
        }
        sug_x += 61;
    }
    cur_y += 80;

    // 4. Emergency Priority Level
    DrawKoreanText("우선순위 (Triage)", label_x, cur_y + 6, FONT_BODY, COL_TEXT);
    // Let's use GuiToggleGroup to avoid dropdown overlays.
    // It returns active index.
    GuiToggleGroup((Rectangle){(float)input_x, (float)cur_y, (float)(input_w / 4), 36.0f},
                   "P1 위험;P2 긴급;P3 준긴급;P4 일반", &reg_level_idx);
    cur_y += 58;

    // 5. Department Type
    DrawKoreanText("배정 진료과", label_x, cur_y + 6, FONT_BODY, COL_TEXT);
    GuiToggleGroup((Rectangle){(float)input_x, (float)cur_y, (float)(input_w / 3), 36.0f},
                   "내과;외과;정형외과", &reg_dept_idx);
    cur_y += 76;

    // Register Button
    Rectangle reg_btn = {(float)(panel_x + (panel_w - 200) / 2), (float)cur_y, 200.0f, 44.0f};
    if (GuiButton(reg_btn, "접수 등록")) {
        if (strlen(reg_name) > 0 && strlen(reg_symptom) > 0) {
            int new_id = id_counter++;
            Patient* p = patient_create(new_id, reg_name, reg_age, reg_symptom,
                                        (EmergencyLevel)(reg_level_idx + 1),
                                        (DepartmentType)(reg_dept_idx + 1), PATIENT_WAITING);

            bool success = false;
            if (reg_level_idx < 3) {
                success = patient_system_register_emergency(system, p);
            } else {
                success = patient_system_register_normal(system, p);
            }

            if (success) {
                // Clear Form
                reg_name[0] = '\0';
                reg_symptom[0] = '\0';
                reg_level_idx = 3;
                reg_dept_idx = 0;
            }
        }
    }
}

// 3. Waiting List View (with Search & Cancel)
void DrawWaitingListView(PatientSystem* system) {
    DrawRectangle(MAIN_AREA_X, MAIN_AREA_Y, MAIN_AREA_W, MAIN_AREA_H, COL_BG);

    static char search_query[64] = "";
    static bool search_edit = false;

    int box_x = MAIN_AREA_X + PADDING_MID;
    int box_w = MAIN_AREA_W - (PADDING_MID * 2);

    // Title & Search Box in a single row
    int header_y = MAIN_AREA_Y + PADDING_MID;
    DrawKoreanText("대기 환자 목록 및 검색", box_x, header_y + 8, FONT_HEADING, COL_TEXT);

    // Search TextBox on the right
    int search_w = 260;
    int search_x = box_x + box_w - search_w;
    DrawKoreanText("검색:", search_x - 45, header_y + 8, FONT_BODY, COL_TEXT_SEC);
    if (GuiTextBox((Rectangle){(float)search_x, (float)header_y, (float)search_w, 32.0f},
                   search_query, 64, search_edit)) {
        search_edit = !search_edit;
    }

    int table_y = header_y + 50;
    int table_h = MAIN_AREA_H - (table_y - MAIN_AREA_Y) - PADDING_MID;

    // Draw Table Box
    DrawRectangleRounded((Rectangle){(float)box_x, (float)table_y, (float)box_w, (float)table_h},
                         0.5f, 4, COL_SURFACE);
    DrawRectangleRoundedLines(
        (Rectangle){(float)box_x, (float)table_y, (float)box_w, (float)table_h}, 0.5f, 4,
        COL_BORDER);

    // Header Background
    DrawRectangle(box_x + 1, table_y + 1, box_w - 2, TABLE_HEADER_HEIGHT, COL_HOVER);

    const char* headers[] = {"ID", "환자명 (나이)", "진료과", "증상", "우선순위", "대기", "조작"};
    int widths[] = {60, 160, 100, 220, 110, 80, 80};
    int header_text_y = table_y + (TABLE_HEADER_HEIGHT - FONT_SMALL) / 2;
    int cur_x = box_x + PADDING_MID;

    for (int i = 0; i < 7; i++) {
        DrawKoreanText(headers[i], cur_x, header_text_y, FONT_SMALL, COL_TEXT_SEC);
        cur_x += widths[i];
    }

    // Filter patients based on search
    Patient* waiting_list[100];
    int raw_count = GetWaitingPatients(system, waiting_list, 100);
    Patient* filtered_list[100];
    int filtered_count = 0;

    bool is_searching = (strlen(search_query) > 0);
    Patient* bst_search_result = NULL;

    if (is_searching) {
        // If query is an ID (all digits)
        bool is_digit = true;
        for (size_t i = 0; i < strlen(search_query); i++) {
            if (search_query[i] < '0' || search_query[i] > '9') {
                is_digit = false;
                break;
            }
        }

        if (is_digit) {
            // Integrate BST Search
            int search_id = atoi(search_query);
            bst_search_result = patient_system_search_by_id(system, search_id);
            if (bst_search_result && bst_search_result->status == PATIENT_WAITING) {
                filtered_list[filtered_count++] = bst_search_result;
            }
        } else {
            // Linear search for Name or Symptom matching query substring
            for (int i = 0; i < raw_count; i++) {
                Patient* p = waiting_list[i];
                if (strstr(p->name, search_query) != NULL ||
                    strstr(p->symptom, search_query) != NULL) {
                    filtered_list[filtered_count++] = p;
                }
            }
        }
    } else {
        // Not searching, copy all
        for (int i = 0; i < raw_count; i++) {
            filtered_list[filtered_count++] = waiting_list[i];
        }
    }

    // Render Rows
    int row_y = table_y + TABLE_HEADER_HEIGHT;
    int max_rows = (table_h - TABLE_HEADER_HEIGHT) / TABLE_ROW_HEIGHT;

    if (filtered_count == 0) {
        if (is_searching) {
            DrawKoreanText("검색 조건에 맞는 대기 환자가 없습니다.", box_x + 30, row_y + 20,
                           FONT_BODY, COL_TEXT_HINT);
        } else {
            DrawKoreanText("대기 중인 환자가 없습니다.", box_x + 30, row_y + 20, FONT_BODY,
                           COL_TEXT_HINT);
        }
    } else {
        for (int i = 0; i < filtered_count && i < max_rows; i++) {
            if (i % 2 == 0) {
                DrawRectangle(box_x + 1, row_y, box_w - 2, TABLE_ROW_HEIGHT, ROW_ALT_COLOR);
            }

            Patient* pat = filtered_list[i];
            int text_y = row_y + (TABLE_ROW_HEIGHT - FONT_BODY) / 2;
            int cell_x = box_x + PADDING_MID;

            // 1. ID
            char id_str[16];
            snprintf(id_str, sizeof(id_str), "%d", pat->id);
            DrawKoreanText(id_str, cell_x, text_y, FONT_BODY, COL_TEXT);
            cell_x += widths[0];

            // 2. Name (Age)
            char name_age[64];
            snprintf(name_age, sizeof(name_age), "%s (%d세)", pat->name, pat->age);
            DrawKoreanText(name_age, cell_x, text_y, FONT_BODY, COL_TEXT);
            cell_x += widths[1];

            // 3. Dept
            DrawKoreanText(GetDeptName(pat->department), cell_x, text_y, FONT_BODY, COL_TEXT);
            cell_x += widths[2];

            // 4. Symptom
            char symptom_trunc[32];
            strncpy(symptom_trunc, pat->symptom, 16);
            symptom_trunc[16] = '\0';
            if (strlen(pat->symptom) > 16) {
                strcat(symptom_trunc, "...");
            }
            DrawKoreanText(symptom_trunc, cell_x, text_y, FONT_BODY, COL_TEXT_SEC);
            cell_x += widths[3];

            // 5. Priority Badge
            Color bg_col = GetPriorityBgColor(pat->emergency_level);
            Color txt_col = GetPriorityTextColor(pat->emergency_level);
            Rectangle badge = {(float)cell_x, (float)(row_y + 6), 90.0f, 24.0f};
            DrawRectangleRounded(badge, 0.3f, 4, bg_col);
            DrawRectangleRoundedLines(badge, 0.3f, 4, txt_col);
            int lbl_w = (int)MeasureTextEx(g_font, GetPriorityLabel(pat->emergency_level),
                                           (float)FONT_SMALL, 2.0f)
                            .x;
            DrawKoreanText(GetPriorityLabel(pat->emergency_level),
                           (int)(badge.x + (badge.width - lbl_w) / 2), (int)(badge.y + 3),
                           FONT_SMALL, txt_col);
            cell_x += widths[4];

            // 6. Wait Time
            char wait_desc[32];
            GetWaitTimeDesc(pat, wait_desc, sizeof(wait_desc));
            DrawKoreanText(wait_desc, cell_x, text_y, FONT_BODY, COL_TEXT);
            cell_x += widths[5];

            // 7. Cancel Button
            Rectangle cancel_btn = {(float)(cell_x + 5), (float)(row_y + 6), 75.0f, 24.0f};
            if (GuiButton(cancel_btn, "취소")) {
                patient_system_remove_by_id(system, pat->id);
            }

            row_y += TABLE_ROW_HEIGHT;
        }
    }
}

// 4. Treating Status View (Detailed ER Beds view)
void DrawTreatingView(PatientSystem* system) {
    DrawRectangle(MAIN_AREA_X, MAIN_AREA_Y, MAIN_AREA_W, MAIN_AREA_H, COL_BG);

    int box_x = MAIN_AREA_X + PADDING_MID;
    int header_y = MAIN_AREA_Y + PADDING_MID;
    DrawKoreanText("병상 상세 진료 현황", box_x, header_y, FONT_HEADING, COL_TEXT);

    int grid_y = header_y + 40;

    // Draw 3x2 Grid for 6 beds
    int card_w = (MAIN_AREA_W - (PADDING_MID * 4)) / 3;
    int card_h = (MAIN_AREA_H - 80 - (PADDING_MID * 3)) / 2;

    for (int i = 0; i < ER_BED_COUNT; i++) {
        int row = i / 3;
        int col = i % 3;
        int card_x = MAIN_AREA_X + PADDING_MID + col * (card_w + PADDING_MID);
        int item_y = grid_y + row * (card_h + PADDING_MID);

        Rectangle card_rect = {(float)card_x, (float)item_y, (float)card_w, (float)card_h};
        Patient* patient = system->beds[i];

        if (patient) {
            // Occupied Bed Card
            DrawRectangleRounded(card_rect, 0.05f, 4, COL_SURFACE);
            DrawRectangleRoundedLines(card_rect, 0.05f, 4,
                                      GetPriorityTextColor(patient->emergency_level));

            // Header bar in card
            DrawRectangle(card_x + 1, item_y + 1, card_w - 2, 32,
                          GetPriorityBgColor(patient->emergency_level));
            char title_buf[64];
            snprintf(title_buf, sizeof(title_buf), "%d번 병상 [%s]", i + 1,
                     GetPriorityLabel(patient->emergency_level));
            DrawKoreanText(title_buf, card_x + 12, item_y + 6, FONT_SMALL,
                           GetPriorityTextColor(patient->emergency_level));

            // Details
            int details_y = item_y + 45;
            char name_age[64];
            snprintf(name_age, sizeof(name_age), "성명: %s (%d세)", patient->name, patient->age);
            DrawKoreanText(name_age, card_x + 12, details_y, FONT_SMALL, COL_TEXT);
            details_y += 22;

            char dept_str[64];
            snprintf(dept_str, sizeof(dept_str), "진료과: %s", GetDeptName(patient->department));
            DrawKoreanText(dept_str, card_x + 12, details_y, FONT_SMALL, COL_TEXT);
            details_y += 22;

            char sym_str[128];
            snprintf(sym_str, sizeof(sym_str), "증상: %s", patient->symptom);
            // truncate symptom if too long
            if (strlen(sym_str) > 28) {
                sym_str[25] = '\0';
                strcat(sym_str, "...");
            }
            DrawKoreanText(sym_str, card_x + 12, details_y, FONT_SMALL, COL_TEXT_SEC);
            details_y += 22;

            // Treat Time (Elapsed time in bed)
            int elapsed_sec =
                (int)(time(NULL) - patient->arrival_time);  // Simplification using arrival_time
            char elapsed_str[64];
            snprintf(elapsed_str, sizeof(elapsed_str), "진료 시간: %d분 %d초", elapsed_sec / 60,
                     elapsed_sec % 60);
            DrawKoreanText(elapsed_str, card_x + 12, details_y, FONT_SMALL, COL_TEXT_SEC);

            // Large Treatment Complete Button at bottom of card
            Rectangle complete_btn = {(float)(card_x + 12), (float)(item_y + card_h - 38),
                                      (float)(card_w - 24), 28.0f};
            if (GuiButton(complete_btn, "진료 완료")) {
                patient_system_finish_treatment(system, i);
            }
        } else {
            // Free Bed Card
            DrawRectangleRounded(card_rect, 0.05f, 4, COL_SURFACE);
            DrawRectangleRoundedLines(card_rect, 0.05f, 4, COL_BORDER);

            // Header bar
            DrawRectangle(card_x + 1, item_y + 1, card_w - 2, 32, COL_HOVER);
            char title_buf[32];
            snprintf(title_buf, sizeof(title_buf), "%d번 병상", i + 1);
            DrawKoreanText(title_buf, card_x + 12, item_y + 6, FONT_SMALL, COL_TEXT_SEC);

            // Empty state illustration
            DrawKoreanText("[빈 병상]", card_x + 12, item_y + 50, FONT_HEADING, COL_ROOM_FREE_TEXT);

            // render line by line
            DrawKoreanText("환자가 배정되지 않았습니다.", card_x + 12, item_y + 85, FONT_SMALL,
                           COL_TEXT_HINT);
            DrawKoreanText("우측의 '다음 환자 호출'을", card_x + 12, item_y + 105, FONT_SMALL,
                           COL_TEXT_HINT);
            DrawKoreanText("통해 대기 환자를 배정하세요.", card_x + 12, item_y + 125, FONT_SMALL,
                           COL_TEXT_HINT);
        }
    }
}

// 5. History View (Vertical Timeline Stack)
void DrawHistoryView(PatientSystem* system) {
    DrawRectangle(MAIN_AREA_X, MAIN_AREA_Y, MAIN_AREA_W, MAIN_AREA_H, COL_BG);

    int box_x = MAIN_AREA_X + PADDING_MID;
    int box_w = MAIN_AREA_W - (PADDING_MID * 2);
    int header_y = MAIN_AREA_Y + PADDING_MID;

    DrawKoreanText("완료 환자 기록 및 되돌리기 (Stack)", box_x, header_y + 8, FONT_HEADING,
                   COL_TEXT);

    // Undo Button
    int undo_w = 160;
    int undo_x = box_x + box_w - undo_w;

    int old_state = GuiGetState();
    if (stack_is_empty(&system->history)) {
        GuiSetState(STATE_DISABLED);
    }
    if (GuiButton((Rectangle){(float)undo_x, (float)header_y, (float)undo_w, 32.0f},
                  "최근 완료 취소 (Undo)")) {
        patient_system_undo_last_treatment(system);
    }
    GuiSetState(old_state);

    int list_y = header_y + 50;
    int list_h = MAIN_AREA_H - (list_y - MAIN_AREA_Y) - PADDING_MID;

    // Draw List Container Box
    DrawRectangleRounded((Rectangle){(float)box_x, (float)list_y, (float)box_w, (float)list_h},
                         0.05f, 4, COL_SURFACE);
    DrawRectangleRoundedLines((Rectangle){(float)box_x, (float)list_y, (float)box_w, (float)list_h},
                              0.05f, 4, COL_BORDER);

    // Walk history stack
    StackNode* current = system->history.top;

    int item_y = list_y + 20;
    int line_x = box_x + 40;

    // Draw timeline vertical line
    if (current) {
        DrawLine(line_x, list_y + 10, line_x, list_y + list_h - 10, COL_BORDER_DARK);
    } else {
        DrawKoreanText("진료를 마치고 완료된 환자가 없습니다.", box_x + 30, list_y + 30, FONT_BODY,
                       COL_TEXT_HINT);
        return;
    }

    int drawn_count = 0;
    while (current && item_y < list_y + list_h - 60) {
        Patient* p = (Patient*)current->data;
        if (p) {
            // Timeline circle
            DrawCircle(line_x, item_y + 16, 8, COL_PRIMARY);
            DrawCircle(line_x, item_y + 16, 5, COL_SURFACE);

            // Patient Card
            int card_x = line_x + 20;
            int card_w = box_w - 90;
            Rectangle row_card = {(float)card_x, (float)item_y, (float)card_w, 48.0f};

            DrawRectangleRounded(row_card, 0.1f, 4, ROW_ALT_COLOR);
            DrawRectangleRoundedLines(row_card, 0.1f, 4, COL_BORDER);

            // Information details
            char info_buf[256];
            snprintf(info_buf, sizeof(info_buf), "[완료 %d]  %s (%d세)  -  %s  |  증상: %s",
                     (int)(system->history.size - drawn_count), p->name, p->age,
                     GetDeptName(p->department), p->symptom);

            // Priority badge in row
            DrawKoreanText(info_buf, card_x + 12, item_y + 14, FONT_SMALL, COL_TEXT);

            // Priority badge on the right
            Rectangle badge_rect = {(float)(card_x + card_w - 110), (float)(item_y + 12), 90.0f,
                                    24.0f};
            DrawRectangleRounded(badge_rect, 0.3f, 4, GetPriorityBgColor(p->emergency_level));
            DrawRectangleRoundedLines(badge_rect, 0.3f, 4,
                                      GetPriorityTextColor(p->emergency_level));
            int lbl_w = (int)MeasureTextEx(g_font, GetPriorityLabel(p->emergency_level),
                                           (float)FONT_SMALL, 2.0f)
                            .x;
            DrawKoreanText(GetPriorityLabel(p->emergency_level),
                           (int)(badge_rect.x + (badge_rect.width - lbl_w) / 2),
                           (int)(badge_rect.y + 3), FONT_SMALL,
                           GetPriorityTextColor(p->emergency_level));

            item_y += 58;
            drawn_count++;
        }
        current = current->next;
    }
}

// 6. Stats View (Department & Priority distribution charts)
void DrawStatsView(PatientSystem* system) {
    DrawRectangle(MAIN_AREA_X, MAIN_AREA_Y, MAIN_AREA_W, MAIN_AREA_H, COL_BG);

    int box_x = MAIN_AREA_X + PADDING_MID;
    int header_y = MAIN_AREA_Y + PADDING_MID;

    DrawKoreanText("응급실 통계 분석", box_x, header_y, FONT_HEADING, COL_TEXT);

    int charts_y = header_y + 50;
    int chart_w = (MAIN_AREA_W - (PADDING_MID * 3)) / 2;
    int chart_h = MAIN_AREA_H - 100;

    // Chart 1: Department distribution (내과, 외과, 정형외과)
    {
        int cx = box_x;
        DrawRectangleRounded(
            (Rectangle){(float)cx, (float)charts_y, (float)chart_w, (float)chart_h}, 0.02f, 4,
            COL_SURFACE);
        DrawRectangleRoundedLines(
            (Rectangle){(float)cx, (float)charts_y, (float)chart_w, (float)chart_h}, 0.02f, 4,
            COL_BORDER);

        DrawKoreanText("진료과별 누적 환자 비율", cx + 20, charts_y + 20, FONT_HEADING, COL_TEXT);

        // Fetch stats
        int count_int = (int)patient_system_get_by_dept(system, DEPARTMENT_INTERNAL);
        int count_surg = (int)patient_system_get_by_dept(system, DEPARTMENT_SURGERY);
        int count_ortho = (int)patient_system_get_by_dept(system, DEPARTMENT_ORTHOPEDICS);
        int total = count_int + count_surg + count_ortho;

        int graph_x = cx + 80;
        int graph_y = charts_y + 100;
        int graph_w = chart_w - 120;
        int graph_h = chart_h - 180;

        // Draw axes
        DrawLine(graph_x, graph_y, graph_x, graph_y + graph_h, COL_TEXT_SEC);
        DrawLine(graph_x, graph_y + graph_h, graph_x + graph_w, graph_y + graph_h, COL_TEXT_SEC);

        const char* labels[] = {"내과", "외과", "정형외과"};
        int values[] = {count_int, count_surg, count_ortho};
        Color bar_colors[] = {COL_PRIMARY, COL_STAT_WARNING, COL_STAT_SUCCESS};

        int max_val = 1;
        for (int i = 0; i < 3; i++) {
            if (values[i] > max_val) max_val = values[i];
        }

        int bar_gap = 40;
        int bar_w = (graph_w - (bar_gap * 4)) / 3;

        for (int i = 0; i < 3; i++) {
            int bar_x = graph_x + bar_gap + i * (bar_w + bar_gap);
            int height = (values[i] * graph_h) / max_val;
            int bar_y = graph_y + graph_h - height;

            // Draw Bar
            DrawRectangle(bar_x, bar_y, bar_w, height, bar_colors[i]);

            // Value text
            char val_buf[16];
            snprintf(val_buf, sizeof(val_buf), "%d명", values[i]);
            int tw = (int)MeasureTextEx(g_font, val_buf, (float)FONT_SMALL, 2.0f).x;
            DrawKoreanText(val_buf, bar_x + (bar_w - tw) / 2, bar_y - 20, FONT_SMALL, COL_TEXT);

            // Label text
            int lw = (int)MeasureTextEx(g_font, labels[i], (float)FONT_SMALL, 2.0f).x;
            DrawKoreanText(labels[i], bar_x + (bar_w - lw) / 2, graph_y + graph_h + 10, FONT_SMALL,
                           COL_TEXT);
        }

        char tot_buf[64];
        snprintf(tot_buf, sizeof(tot_buf), "총 환자수: %d명", total);
        DrawKoreanText(tot_buf, cx + 20, charts_y + chart_h - 35, FONT_SMALL, COL_TEXT_SEC);
    }

    // Chart 2: Priority level distribution (P1 ~ P4)
    {
        int cx = box_x + chart_w + PADDING_MID;
        DrawRectangleRounded(
            (Rectangle){(float)cx, (float)charts_y, (float)chart_w, (float)chart_h}, 0.02f, 4,
            COL_SURFACE);
        DrawRectangleRoundedLines(
            (Rectangle){(float)cx, (float)charts_y, (float)chart_w, (float)chart_h}, 0.02f, 4,
            COL_BORDER);

        DrawKoreanText("우선순위(Triage) 환자 분포", cx + 20, charts_y + 20, FONT_HEADING,
                       COL_TEXT);

        // Fetch stats
        int p1 = (int)patient_system_get_by_priority(system, EMERGENCY_LEVEL_1);
        int p2 = (int)patient_system_get_by_priority(system, EMERGENCY_LEVEL_2);
        int p3 = (int)patient_system_get_by_priority(system, EMERGENCY_LEVEL_3);
        int p4 = (int)patient_system_get_by_priority(system, EMERGENCY_LEVEL_4);

        int graph_x = cx + 80;
        int graph_y = charts_y + 100;
        int graph_w = chart_w - 120;
        int graph_h = chart_h - 180;

        // Draw axes
        DrawLine(graph_x, graph_y, graph_x, graph_y + graph_h, COL_TEXT_SEC);
        DrawLine(graph_x, graph_y + graph_h, graph_x + graph_w, graph_y + graph_h, COL_TEXT_SEC);

        const char* labels[] = {"P1", "P2", "P3", "P4"};
        int values[] = {p1, p2, p3, p4};
        Color bar_colors[] = {COL_STAT_CRITICAL, COL_STAT_WARNING, COL_PRIMARY, COL_STAT_SUCCESS};

        int max_val = 1;
        for (int i = 0; i < 4; i++) {
            if (values[i] > max_val) max_val = values[i];
        }

        int bar_gap = 25;
        int bar_w = (graph_w - (bar_gap * 5)) / 4;

        for (int i = 0; i < 4; i++) {
            int bar_x = graph_x + bar_gap + i * (bar_w + bar_gap);
            int height = (values[i] * graph_h) / max_val;
            int bar_y = graph_y + graph_h - height;

            // Draw Bar
            DrawRectangle(bar_x, bar_y, bar_w, height, bar_colors[i]);

            // Value text
            char val_buf[16];
            snprintf(val_buf, sizeof(val_buf), "%d명", values[i]);
            int tw = (int)MeasureTextEx(g_font, val_buf, (float)FONT_SMALL, 2.0f).x;
            DrawKoreanText(val_buf, bar_x + (bar_w - tw) / 2, bar_y - 20, FONT_SMALL, COL_TEXT);

            // Label text
            int lw = (int)MeasureTextEx(g_font, labels[i], (float)FONT_SMALL, 2.0f).x;
            DrawKoreanText(labels[i], bar_x + (bar_w - lw) / 2, graph_y + graph_h + 10, FONT_SMALL,
                           COL_TEXT);
        }
    }
}
