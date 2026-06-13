#ifndef ER_RIGHT_PANEL_H
#define ER_RIGHT_PANEL_H

#include "systems/patient_system.h"

void DrawRightPanel(PatientSystem* system);
void RightPanel_HandleClick(PatientSystem* system, int mouse_x, int mouse_y);

#endif // ER_RIGHT_PANEL_H
