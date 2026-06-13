#ifndef SYSTEMS_PATIENT_SYSTEM_H
#define SYSTEMS_PATIENT_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>

#include "structures/list.h"
#include "structures/priority_queue.h"
#include "structures/queue.h"
#include "structures/stack.h"
#include "structures/tree.h"
#include "systems/patient.h"

#define ER_BED_COUNT 6

typedef struct {
    LinkedList patients;
    Queue normal_queue;
    PriorityQueue emergency_queue;
    Stack history;
    BSTNode* index_root;
    Patient* beds[ER_BED_COUNT];
} PatientSystem;

void patient_system_init(PatientSystem* system);

void patient_system_clear(PatientSystem* system);

bool patient_system_register_normal(PatientSystem* system, Patient* patient);

bool patient_system_register_emergency(PatientSystem* system, Patient* patient);

Patient* patient_system_call_next(PatientSystem* system);

Patient* patient_system_search_by_id(const PatientSystem* system, int id);

Patient* patient_system_search_by_name(const PatientSystem* system, const char* name);

bool patient_system_remove_by_id(PatientSystem* system, int id);

size_t patient_system_total(const PatientSystem* system);

size_t patient_system_waiting_normal(const PatientSystem* system);

size_t patient_system_waiting_emergency(const PatientSystem* system);

size_t patient_system_get_by_priority(const PatientSystem* system, EmergencyLevel level);

size_t patient_system_get_by_dept(const PatientSystem* system, DepartmentType dept);

bool patient_system_finish_treatment(PatientSystem* system, int bed_index);

bool patient_system_undo_last_treatment(PatientSystem* system);

#endif
