#ifndef ER_MAIN_VIEW_H
#define ER_MAIN_VIEW_H

#include "raylib.h"
#include "systems/patient_system.h"

void DrawDashboardView(PatientSystem* system);
void DrawRegisterView(PatientSystem* system);
void DrawWaitingListView(PatientSystem* system);
void DrawTreatingView(PatientSystem* system);
void DrawHistoryView(PatientSystem* system);
void DrawStatsView(PatientSystem* system);

// Helper function declarations
void DrawStatCard(int pos_x, int pos_y, int width, int height, const char* label, int value, Color value_color);

#endif  // ER_MAIN_VIEW_H