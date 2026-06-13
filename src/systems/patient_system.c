#include "systems/patient_system.h"

#include <stdlib.h>

static bool queue_remove_by_id(Queue* queue, int id) {
    QueueNode* prev = NULL;
    QueueNode* current = NULL;

    if (!queue) {
        return false;
    }

    current = queue->front;
    while (current) {
        Patient* patient = (Patient*)current->data;
        if (patient && patient->id == id) {
            if (prev) {
                prev->next = current->next;
            } else {
                queue->front = current->next;
            }

            if (current == queue->rear) {
                queue->rear = prev;
            }

            queue->size -= 1;
            free(current);
            return true;
        }

        prev = current;
        current = current->next;
    }

    return false;
}

static bool priority_queue_remove_by_id(PriorityQueue* queue, int id) {
    PriorityQueueNode* prev = NULL;
    PriorityQueueNode* current = NULL;

    if (!queue) {
        return false;
    }

    current = queue->front;
    while (current) {
        Patient* patient = (Patient*)current->data;
        if (patient && patient->id == id) {
            if (prev) {
                prev->next = current->next;
            } else {
                queue->front = current->next;
            }

            queue->size -= 1;
            free(current);
            return true;
        }

        prev = current;
        current = current->next;
    }

    return false;
}

void patient_system_init(PatientSystem* system) {
    if (!system) {
        return;
    }

    list_init(&system->patients);
    queue_init(&system->normal_queue);
    priority_queue_init(&system->emergency_queue);
    stack_init(&system->history);
    system->index_root = NULL;
    for (int i = 0; i < ER_BED_COUNT; i++) {
        system->beds[i] = NULL;
    }
}

void patient_system_clear(PatientSystem* system) {
    if (!system) {
        return;
    }

    queue_clear(&system->normal_queue, NULL);
    priority_queue_clear(&system->emergency_queue, NULL);
    list_clear(&system->patients, (void (*)(void*))patient_destroy);
    stack_clear(&system->history, NULL);
    bst_clear(system->index_root);
    system->index_root = NULL;
}

static bool patient_system_register_common(PatientSystem* system, Patient* patient) {
    if (!system || !patient) {
        return false;
    }

    if (patient_system_search_by_id(system, patient->id)) {
        return false;
    }

    if (!list_push_back(&system->patients, patient)) {
        return false;
    }

    system->index_root = bst_insert(system->index_root, patient);

    patient->status = PATIENT_WAITING;
    return true;
}

bool patient_system_register_normal(PatientSystem* system, Patient* patient) {
    if (!patient_system_register_common(system, patient)) {
        return false;
    }

    if (!enqueue(&system->normal_queue, patient)) {
        list_remove(&system->patients, patient_compare_id, &patient->id, NULL);
        system->index_root = bst_delete(system->index_root, patient->id);
        patient_destroy(patient);
        return false;
    }

    return true;
}

bool patient_system_register_emergency(PatientSystem* system, Patient* patient) {
    if (!patient_system_register_common(system, patient)) {
        return false;
    }

    if (!priority_enqueue(&system->emergency_queue, patient, patient->emergency_level)) {
        list_remove(&system->patients, patient_compare_id, &patient->id, NULL);
        system->index_root = bst_delete(system->index_root, patient->id);
        patient_destroy(patient);
        return false;
    }

    return true;
}

Patient* patient_system_call_next(PatientSystem* system) {
    Patient* patient = NULL;

    if (!system) {
        return NULL;
    }

    // Find first empty bed
    int empty_bed_idx = -1;
    for (int i = 0; i < ER_BED_COUNT; i++) {
        if (system->beds[i] == NULL) {
            empty_bed_idx = i;
            break;
        }
    }

    // If all beds are full, we cannot call next patient
    if (empty_bed_idx == -1) {
        return NULL;
    }

    if (!priority_queue_is_empty(&system->emergency_queue)) {
        patient = (Patient*)priority_dequeue(&system->emergency_queue);
    } else {
        patient = (Patient*)dequeue(&system->normal_queue);
    }

    if (patient) {
        patient->status = PATIENT_IN_TREATMENT;
        system->beds[empty_bed_idx] = patient;
    }

    return patient;
}

Patient* patient_system_search_by_id(const PatientSystem* system, int id) {
    if (!system) {
        return NULL;
    }

    return bst_search(system->index_root, id);
}

Patient* patient_system_search_by_name(const PatientSystem* system, const char* name) {
    if (!system || !name) {
        return NULL;
    }

    return (Patient*)list_find(&system->patients, patient_compare_name, name);
}

bool patient_system_remove_by_id(PatientSystem* system, int id) {
    bool removed = false;

    if (!system) {
        return false;
    }

    queue_remove_by_id(&system->normal_queue, id);
    priority_queue_remove_by_id(&system->emergency_queue, id);
    system->index_root = bst_delete(system->index_root, id);

    removed =
        list_remove(&system->patients, patient_compare_id, &id, (void (*)(void*))patient_destroy);
    return removed;
}

size_t patient_system_total(const PatientSystem* system) {
    if (!system) {
        return 0;
    }

    return system->patients.size;
}

size_t patient_system_waiting_normal(const PatientSystem* system) {
    if (!system) {
        return 0;
    }

    return system->normal_queue.size;
}

size_t patient_system_waiting_emergency(const PatientSystem* system) {
    if (!system) {
        return 0;
    }

    return system->emergency_queue.size;
}

size_t patient_system_get_by_priority(const PatientSystem* system, EmergencyLevel level) {
    if (!system) {
        return 0;
    }

    size_t count = 0;
    ListNode* current = system->patients.head;
    while (current) {
        Patient* pat = (Patient*)current->data;
        if (pat && pat->emergency_level == level) {
            count++;
        }
        current = current->next;
    }
    return count;
}

size_t patient_system_get_by_dept(const PatientSystem* system, DepartmentType dept) {
    if (!system) {
        return 0;
    }

    size_t count = 0;
    ListNode* current = system->patients.head;
    while (current) {
        Patient* pat = (Patient*)current->data;
        if (pat && pat->department == dept) {
            count++;
        }
        current = current->next;
    }
    return count;
}

bool patient_system_finish_treatment(PatientSystem* system, int bed_index) {
    if (!system || bed_index < 0 || bed_index >= ER_BED_COUNT) {
        return false;
    }

    Patient* patient = system->beds[bed_index];
    if (!patient) {
        return false;
    }

    patient->status = PATIENT_FINISHED;
    stack_push(&system->history, patient);
    system->beds[bed_index] = NULL;
    return true;
}

bool patient_system_undo_last_treatment(PatientSystem* system) {
    if (!system || stack_is_empty(&system->history)) {
        return false;
    }

    Patient* patient = (Patient*)stack_pop(&system->history);
    if (!patient) {
        return false;
    }

    patient->status = PATIENT_WAITING;

    if (patient->emergency_level == EMERGENCY_LEVEL_4) {
        enqueue(&system->normal_queue, patient);
    } else {
        priority_enqueue(&system->emergency_queue, patient, patient->emergency_level);
    }

    return true;
}
