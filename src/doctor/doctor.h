#ifndef DOCTOR_H
#define DOCTOR_H

#include <time.h>

#define MAX_DOCTORS 100
#define MAX_PASSWORD_LENGTH 20
#define MAX_LOGIN_ATTEMPTS 3
#define MAX_ASSIGNED_PATIENTS 50

typedef enum
{
    DOCTOR_ACTIVE,
    DOCTOR_DEACTIVE,
    DOCTOR_BLOCK
} doctorStatus;

typedef struct
{
    int d_age;
    char d_id[16];
    char d_name[50];
    char d_cnic[15];
    char d_phone[15];
    char d_gender[10];
    char d_specialization[50];

    char password[MAX_PASSWORD_LENGTH];
    time_t registration_time;
    doctorStatus status;
    int failed_login_attempts;
    int current_patients;
    int assignedPatientIds[MAX_ASSIGNED_PATIENTS]; // stores patient IDs
    int assignedPatientCount;            // how many are assigned
} Doctor;

extern Doctor doctors[MAX_DOCTORS];
extern int doctor_counter;

// Core Functions
void doctorModule();

// Add Doctor
void addDoctor();

// Search Functions
void searchDoctor();
void searchDoctorById();
void searchDoctorByName();
void searchDoctorBySpecialization();
void searchDoctorByCnic();

// Delete Functions
void deleteDoctor();
void deleteDoctorById();
void deleteDoctorByCnic();
// Update functions

void updateDoctorById();
void updateDoctorByCnic();
void updateDoctorByName();
void updateDoctorHelper(int select_index);
void updateDoctorMenu();

// Menu Functions
void displayDoctors();
void displayDoctorMenu();
void displayActiveDoctors();
void displayDeactiveDoctors();
void displayAllDoctors();

// File Operations
void saveDoctorsToFile();
void loadDoctorsFromFile();

#endif
