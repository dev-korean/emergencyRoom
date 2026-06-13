#ifndef SYSTEMS_PATIENT_H
#define SYSTEMS_PATIENT_H

#include <stdbool.h>
#include <time.h>

#define PATIENT_NAME_MAX 64

#define PATIENT_SYMPTOM_MAX 128

typedef enum {
    EMERGENCY_LEVEL_1 = 1, // P1 (Critical)
    EMERGENCY_LEVEL_2 = 2, // P2 (Emergency)
    EMERGENCY_LEVEL_3 = 3, // P3 (Urgent)
    EMERGENCY_LEVEL_4 = 4  // P4 (General)
} EmergencyLevel;

typedef enum {
    DEPARTMENT_INTERNAL = 1,
    DEPARTMENT_SURGERY = 2,
    DEPARTMENT_ORTHOPEDICS = 3
} DepartmentType;

typedef enum { PATIENT_WAITING = 0, PATIENT_IN_TREATMENT = 1, PATIENT_FINISHED = 2 } PatientStatus;

typedef struct {
    int id;
    char name[PATIENT_NAME_MAX];
    int age;
    char symptom[PATIENT_SYMPTOM_MAX];
    EmergencyLevel emergency_level;
    DepartmentType department;
    PatientStatus status;
    time_t arrival_time;
} Patient;

void patient_init(Patient* patient, int id, const char* name, int age, const char* symptom,
                  EmergencyLevel emergency_level, DepartmentType department, PatientStatus status);

Patient* patient_create(int id, const char* name, int age, const char* symptom, EmergencyLevel emergency_level,
                        DepartmentType department, PatientStatus status);

void patient_destroy(Patient* patient);

int patient_compare_id(const void* item, const void* key);

int patient_compare_name(const void* item, const void* key);

#endif
