#include "systems/patient.h"

#include <stdlib.h>
#include <string.h>

static void patient_copy_name(char* dest, const char* src) {
    if (!dest) {
        return;
    }

    if (!src) {
        dest[0] = '\0';
        return;
    }

    strncpy(dest, src, PATIENT_NAME_MAX - 1);
    dest[PATIENT_NAME_MAX - 1] = '\0';
}

void patient_init(Patient* patient, int id, const char* name, int age, const char* symptom,
                  EmergencyLevel emergency_level, DepartmentType department, PatientStatus status) {
    if (!patient) {
        return;
    }

    patient->id = id;
    patient_copy_name(patient->name, name);
    patient->age = age;
    
    if (symptom) {
        strncpy(patient->symptom, symptom, PATIENT_SYMPTOM_MAX - 1);
        patient->symptom[PATIENT_SYMPTOM_MAX - 1] = '\0';
    } else {
        patient->symptom[0] = '\0';
    }

    patient->emergency_level = emergency_level;
    patient->department = department;
    patient->status = status;
    patient->arrival_time = time(NULL);
}

Patient* patient_create(int id, const char* name, int age, const char* symptom, EmergencyLevel emergency_level,
                        DepartmentType department, PatientStatus status) {
    Patient* patient = (Patient*)malloc(sizeof(Patient));
    if (!patient) {
        return NULL;
    }

    patient_init(patient, id, name, age, symptom, emergency_level, department, status);
    return patient;
}

void patient_destroy(Patient* patient) {
    free(patient);
}

int patient_compare_id(const void* item, const void* key) {
    const Patient* patient = (const Patient*)item;
    const int* id = (const int*)key;

    if (!patient || !id) {
        return -1;
    }

    if (patient->id == *id) {
        return 0;
    }

    return (patient->id < *id) ? -1 : 1;
}

int patient_compare_name(const void* item, const void* key) {
    const Patient* patient = (const Patient*)item;
    const char* name = (const char*)key;

    if (!patient || !name) {
        return -1;
    }

    return strcmp(patient->name, name);
}
