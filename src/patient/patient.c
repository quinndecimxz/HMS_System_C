#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include "src/doctor/doctor.h"

#include "src/common/common.h"
#include "src/id_manager/id_manager.h"
#include "src/VisitLog/visitlog.h"
#include "patient.h"
#include "doctor.h"

Patient patients[MAX_PATIENTS];
int patient_counter = 0;

// this is main patient module
void patientModule()
{

    loadPatientFromFile(); // Loading patient data and IDs from file when program starts

    loadIDManager();

    int choice;
    do
    {
        displayPatientMenu();
        choice = inputInt("Enter your choice: ");

        switch (choice)
        {
        case 1:
            addPatient();
            break;
        case 2:
            displayPatient();
            break;
        case 3:
        {
            int search_choice;
            printf("Search Patient by:\n1. ID\n2. Name\n3. CNIC\n4.<<<<<Go Back\n");
            search_choice = inputInt("Enter choice: ");
            while (search_choice != 1 && search_choice != 2 && search_choice != 3 && search_choice != 4)
            {
                printf("Please choose correct option!");
                printf("1. ID\n2. Name\n3. CNIC\n4.<<<<<Go Back\n");
                search_choice = inputInt("Enter choice: ");
            }
            if (search_choice == 1)
                searchPatientById();
            else if (search_choice == 2)
                searchPatientByName();
            else if (search_choice == 3)
                searchPatientByCnic();
            else if (search_choice == 4)
                continue;
            else
                printf("Invalid search choice!\n");
        }
        break;
        case 4:
        {
            int choice;

            printf("\nUpdate Patient by:\n1. ID\n2. Name\n3. CNIC\n4.<<<<<Go Back\n");
            choice = inputInt("Enter choice: ");
            while (choice != 1 && choice != 2 && choice != 3 && choice != 4)
            {
                printf("Please choose correct option!");
                printf("1. ID\n2. Name\n3. CNIC\n4.<<<<<Go Back\n");
                choice = inputInt("Enter choice: ");
            }
            if (choice == 1)
                updatePatientById();
            else if (choice == 2)
                updatePatientByName();
            else if (choice == 3)
                updatePatientByCnic();
            else if (choice == 4)
                continue;
            else
                printf("Invalid search choice!\n");
            break;
        }
        case 5:
            deletePatient();
            break;
        case 6:
            exitProgram();
            break;
        default:
            printf("Invalid choice! Please enter a number between 1 and 5.\n");
        }

        // printf("\nPress Enter to continue...");
        // while (getchar() != '\n';
    } while (choice != 6);
}

// CORE FUNCTIONS

// ------------  Add Patient ----------------
void addPatient()
{
    char choice[10];
    int sub_choice;
    loadDoctorsFromFile();
    do
    {
        sub_choice = inputInt("Want to : \n1.Add patient \n2.<<<<Go Back\n\nConfirm by Choice:");
        if (sub_choice != 1 && sub_choice != 2)
        {
            printf("Choose Correct Option: ");
        }
    } while (sub_choice != 1 && sub_choice != 2);

    // checking max patients limit
    if (sub_choice == 1)
    {
        do
        {
            if (patient_counter >= MAX_PATIENTS)
            {
                printf("List is full\n");
                return;
            }

            // Ask if this is first visit
            char first_visit[10];
            do
            {
                printf("Is this your first visit to our hospital? (Y/N): ");
                inputString(first_visit, sizeof(first_visit));
            } while (!(strcasecmp(first_visit, "y") == 0 || strcasecmp(first_visit, "yes") == 0 ||
                       strcasecmp(first_visit, "n") == 0 || strcasecmp(first_visit, "no") == 0));

            if (strcasecmp(first_visit, "n") == 0 || strcasecmp(first_visit, "no") == 0)
            {
                // Returning Patient Flow
                char search_cnic[15];
                printf("Enter your CNIC (or guardian's CNIC for minors): ");
                inputValidatedCNIC(search_cnic, sizeof(search_cnic));

                int found_index = -1;
                int patient_status = -1;

                // Search for patient by CNIC
                for (int i = 0; i < patient_counter; i++)
                {
                    if ((strlen(patients[i].p_cnic) > 0 && strcmp(patients[i].p_cnic, search_cnic) == 0) ||
                        (strlen(patients[i].guardian_cnic) > 0 && strcmp(patients[i].guardian_cnic, search_cnic) == 0))
                    {
                        found_index = i;
                        patient_status = patients[i].status;
                        break;
                    }
                }
                // if not found ask him to register new patient
                if (found_index == -1)
                {
                    printf("Patient not found with CNIC: %s\n", search_cnic);
                    char register_choice[10];
                    do
                    {
                        printf("Would you like to register as a new patient? (Y/N): ");
                        inputString(register_choice, sizeof(register_choice));
                    } while (!(strcasecmp(register_choice, "y") == 0 || strcasecmp(register_choice, "yes") == 0 ||
                               strcasecmp(register_choice, "n") == 0 || strcasecmp(register_choice, "no") == 0));

                    if (strcasecmp(register_choice, "n") == 0 || strcasecmp(register_choice, "no") == 0)
                    {

                        printf(" do you want to Go back to main menu or add new patient \n");
                        int go_back_choice = inputInt("1. Go back to main menu\n2. Add new patient\nEnter your choice: ");
                        if (go_back_choice == 1)
                        {
                            return; // Go back to main loop
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
                // if found patient and active
                else if (patient_status == PATIENT_ACTIVE)
                {
                    printf("Welcome back, %s! (ID: %s)\n", patients[found_index].p_name, patients[found_index].patient_id);
                    printf("Patient is already active in the system.\n");
                    visitLogMenu(found_index); // provide access to visit log
                    return;
                }
                // if find patient but deactivated, asks for activation
                else if (patient_status == PATIENT_DEACTIVE)
                {
                    char reactivate_choice[10];
                    do
                    {
                        printf("Patient %s (ID: %s) is currently deactivated.\n",
                               patients[found_index].p_name, patients[found_index].patient_id);
                        printf("Do you want to reactivate this patient? (Y/N): ");
                        inputString(reactivate_choice, sizeof(reactivate_choice));
                    } while (!(strcasecmp(reactivate_choice, "y") == 0 || strcasecmp(reactivate_choice, "yes") == 0 ||
                               strcasecmp(reactivate_choice, "n") == 0 || strcasecmp(reactivate_choice, "no") == 0));

                    if (strcasecmp(reactivate_choice, "y") == 0 || strcasecmp(reactivate_choice, "yes") == 0)
                    {
                        patients[found_index].status = PATIENT_ACTIVE;
                        patients[found_index].registration_time = time(NULL);
                        savePatientsToFile();
                        printf("Patient %s (%s) has been successfully reactivated!\n",
                               patients[found_index].p_name, patients[found_index].patient_id);
                    }
                    continue; // Go back to main loop
                }
                // else if (patient_status == PATIENT_DISCHARGED)
                // {
                //     printf("Patient %s (ID: %s) has been discharged.\n",
                //            patients[found_index].p_name, patients[found_index].patient_id);
                //     printf("Please contact administration for further assistance.\n");
                //     continue; // Go back to main loop
                // }
            }

            // if a first visit
            Patient new_patient;

            // to clear all fields in new patient struct
            memset(&new_patient, 0, sizeof(Patient));

            printf("\nNEW PATIENT REGISTRATION\n");
            printf("========================\n");

            // Get patient information
            inputValidatedName(new_patient.p_name, sizeof(new_patient.p_name));
            new_patient.p_age = inputValidatedAge();
            inputValidatedGender(new_patient.p_gender, sizeof(new_patient.p_gender));
            inputValidatedDisease(new_patient.p_disease, sizeof(new_patient.p_disease));
            inputValidatedContact(new_patient.p_contact_num, sizeof(new_patient.p_contact_num));
            inputValidatedBloodGroup(new_patient.p_blood_group, sizeof(new_patient.p_blood_group));
            char cnic_to_check[15] = "";

            printf("\nAvailable Doctors:\n");
            printf("ID\tName\t\tSpecialization\t\tCurrent/Max Patients\tStatus\n");
            for (int i = 0; i < doctor_counter; i++) {
                if (doctors[i].status == DOCTOR_ACTIVE && doctors[i].current_patients < MAX_ASSIGNED_PATIENTS) {
                    printf("%d\t%s\t\t%s\t\t%d/%d\t\tActive\n", 
                        doctors[i].d_id, 
                        doctors[i].d_name, 
                        doctors[i].d_specialization, 
                        doctors[i].current_patients);
                }
            }
            int assignedDoctorId;
            printf("Enter the ID of the doctor to assign: ");
            scanf("%d", &assignedDoctorId);
            new_patient.assigned_doctor_id = assignedDoctorId;
            
            for (int i = 0; i < doctor_counter; i++) {
                if (doctors[i].d_id == assignedDoctorId) {
                    doctors[i].current_patients++;
                    break;
                }
            }
            saveDoctorsToFile();




            // Handle CNIC based on age
            if (new_patient.p_age < 18)
            {
                new_patient.is_minor = 1;
                new_patient.p_cnic[0] = '\0';
                new_patient.guardian_cnic[0] = '\0';

                char has_cnic[10];
                // asks if minor has CNIC or guardian CNIC and assign it accordingly
                do
                {
                    printf("Does the minor have a CNIC? (Y/N): ");
                    inputString(has_cnic, sizeof(has_cnic));
                } while (!(strcasecmp(has_cnic, "y") == 0 || strcasecmp(has_cnic, "yes") == 0 ||
                           strcasecmp(has_cnic, "n") == 0 || strcasecmp(has_cnic, "no") == 0));

                if (strcasecmp(has_cnic, "y") == 0 || strcasecmp(has_cnic, "yes") == 0)
                {
                    printf("Enter minor's CNIC:\n");
                    inputValidatedCNIC(new_patient.p_cnic, sizeof(new_patient.p_cnic));
                    strcpy(cnic_to_check, new_patient.p_cnic);
                }
                else
                {
                    printf("Enter guardian CNIC:\n");
                    inputValidatedCNIC(new_patient.guardian_cnic, sizeof(new_patient.guardian_cnic));
                    strcpy(cnic_to_check, new_patient.guardian_cnic);
                }
            }
            else
            {
                new_patient.is_minor = 0;
                new_patient.guardian_cnic[0] = '\0';
                inputValidatedCNIC(new_patient.p_cnic, sizeof(new_patient.p_cnic));
                strcpy(cnic_to_check, new_patient.p_cnic);
            }

            // CNIC Validation Logic

            int active_patient_index = -1;
            int deactive_patient_index = -1;
            int is_guardian_of_existing_adult = 0;

            for (int i = 0; i < patient_counter; i++)
            {
                // Special case: If the new patient is a minor using a guardian's CNIC,
                // and if one of the existing patients is an adult (not a minor) whose CNIC matches the provided CNIC,
                // then link the minor to the existing adult guardian.
                if (new_patient.is_minor && strlen(new_patient.guardian_cnic) > 0 &&
                    !patients[i].is_minor && strlen(patients[i].p_cnic) > 0 &&
                    strcmp(patients[i].p_cnic, cnic_to_check) == 0)
                {
                    printf("Guardian CNIC %s belongs to existing adult patient %s (ID: %s).\n",
                           cnic_to_check, patients[i].p_name, patients[i].patient_id);
                    printf("Minor will be linked to this guardian.\n");
                    is_guardian_of_existing_adult = 1;
                    new_patient.guardian_cnic[0] = '\0';
                }
                // Regular CNIC conflict check
                else if (
                    (strlen(patients[i].p_cnic) > 0 && strcmp(patients[i].p_cnic, cnic_to_check) == 0 && ((new_patient.is_minor && strlen(new_patient.p_cnic) > 0) || !new_patient.is_minor)) ||
                    (strlen(patients[i].guardian_cnic) > 0 && strcmp(patients[i].guardian_cnic, cnic_to_check) == 0 && new_patient.is_minor && strlen(new_patient.guardian_cnic) > 0))

                {
                    if (patients[i].status == PATIENT_ACTIVE)
                    {
                        // If an active patient with the same CNIC is found, set the index
                        active_patient_index = i;
                        break;
                    }
                    else if (patients[i].status == PATIENT_DEACTIVE)
                    {
                        // If a deactivated patient with the same CNIC is found, set the index
                        deactive_patient_index = i;
                    }
                }
            }

            // If active patient found with same cnic, show error

            if (active_patient_index != -1 && !is_guardian_of_existing_adult)
            {
                printf("CNIC %s is already assigned to active patient %s (ID: %s).\n",
                       cnic_to_check, patients[active_patient_index].p_name, patients[active_patient_index].patient_id);
                printf("Cannot register. Patient already exists in system.\n");
                continue;
            }
            // If deactivated patient found with same cnic, ask to reactivate
            if (deactive_patient_index != -1 && !is_guardian_of_existing_adult)
            {
                char reactivate_choice[10];
                do
                {
                    printf("CNIC %s is assigned to deactivated patient %s (ID: %s).\n",
                           cnic_to_check, patients[deactive_patient_index].p_name, patients[deactive_patient_index].patient_id);
                    printf("Do you want to reactivate this patient? (Y/N): ");
                    inputString(reactivate_choice, sizeof(reactivate_choice));
                } while (!(strcasecmp(reactivate_choice, "y") == 0 || strcasecmp(reactivate_choice, "yes") == 0 ||
                           strcasecmp(reactivate_choice, "n") == 0 || strcasecmp(reactivate_choice, "no") == 0));

                if (strcasecmp(reactivate_choice, "y") == 0 || strcasecmp(reactivate_choice, "yes") == 0)
                {
                    patients[deactive_patient_index].status = PATIENT_ACTIVE;
                    patients[deactive_patient_index].registration_time = time(NULL);
                    savePatientsToFile();
                    printf("Patient %s (%s) has been successfully reactivated!\n",
                           patients[deactive_patient_index].p_name, patients[deactive_patient_index].patient_id);
                }
                    

                printf("Do you want to add another patient or go back to the main menu?\n");
                int go_back_choice = inputInt("1. Go back to main menu\n2. Add new patient\nEnter your choice: ");
                if (go_back_choice == 2)
                {
                    continue;
                }
                else
                {
                    return;
                }
            }

            // No matching CNIC found - create new patient
            snprintf(new_patient.patient_id, sizeof(new_patient.patient_id), "P%05d", patient_counter + 1);

            new_patient.registration_time = time(NULL);
            new_patient.status = PATIENT_ACTIVE;

            // Add the new patient
            patients[patient_counter++] = new_patient;
            savePatientsToFile();
            printf("Patient Entered Successfully!\n");
            printf("Patient ID: %s\n", new_patient.patient_id);
            printf("Name: %s\n", new_patient.p_name);

            printf("Do you want to add another patient (Y/N)? ");
            inputString(choice, sizeof(choice));

        } while (strcasecmp(choice, "n") != 0 && strcasecmp(choice, "no") != 0);
    }
    else if (sub_choice == 2)
    {
        return;
    }
}

//  ---------------------------------------Search Functions
void searchPatientById()

{

    int confirm = inputInt("1. Search By ID\n2. <<<<<Go back\nEnter your choice: ");
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }

    int found = 0;
    int i;
    char id[20];
    printf("Enter Id of patient : ");
    inputString(id, sizeof(id));

    printf("-------------------------------------------------------------------------------------------------------\n");
    printf("| %-5s | %-20s | %-3s | %-6s | %-15s | %-15s | %-15s | %-15s |\n", "ID", "Name", "Age", "Blood Group", "Gender", "Disease", "Contact", "Registration D&T");
    printf("-------------------------------------------------------------------------------------------------------\n");
    for (i = 0; i < patient_counter; i++)
    {
        if (strcmp(patients[i].patient_id, id) == 0 && patients[i].status == PATIENT_ACTIVE)
        {

            char reg_time_str[25];
            time_t currentTime = time(NULL);
            formatRegistrationTime(currentTime, reg_time_str, sizeof(reg_time_str));

            printf("| %-5s | %-20s | %-3d | %-6s | %-15s | %-15s | %-19s | %-15s |\n",
                   patients[i].patient_id, patients[i].p_name, patients[i].p_age, patients[i].p_blood_group,
                   patients[i].p_gender, patients[i].p_disease, patients[i].p_contact_num, reg_time_str);

            found = 1;
            printf("-------------------------------------------------------------------------------------------------------\n");

            break;
        }
    }
    if (!found)
    {
        printf("Patient with ID %s not found.\n", id);
    }
    else
    {
        visitLogMenu(i);
    }
}

void searchPatientByName()
{
    int confirm = inputInt("1. Search By Name\n2. Go back\nEnter your choice: ");
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }

    char name[50];
    printf("Enter Name: ");
    inputString(name, sizeof(name));

    int matches[100];
    int matchCount = 0;

    // Case-insensitive prefix match
    for (int i = 0; i < patient_counter; i++)
    {
        if (patients[i].status == PATIENT_ACTIVE &&
            strncasecmp(patients[i].p_name, name, strlen(name)) == 0)
        {
            matches[matchCount++] = i;
        }
    }

    if (matchCount == 0)
    {
        printf("\nNo active patients found matching '%s'.\n", name);
        return;
    }

    int select_index = -1;

    while (1)
    {
        // if find only one match
        if (matchCount == 1)
        {
            select_index = matches[0];

            visitLogMenu(select_index);
        }
        else
        {
            // if found multiple matches ask user to select one
            printf("\nMultiple patients found with matching name:\n");
            printf("---------------------------------------------------------------------------------------------\n");
            printf(" %-5s | %-5s |%-20s | %-3s | %-10s | %-15s | %-15s | %-15s |\n",
                   "No.", "ID", "Name", "Age", "Gender", "Blood Group", "Disease", "Contact");

            for (int i = 0; i < matchCount; i++)
            {
                int idx = matches[i];
                printf(" %-5d | %-5s |%-20s | %-3d | %-10s | %-15s | %-15s | %-15s |\n",
                       i + 1, patients[idx].patient_id, patients[idx].p_name, patients[idx].p_age,
                       patients[idx].p_gender, patients[idx].p_blood_group, patients[idx].p_disease, patients[idx].p_contact_num);
            }
            printf("---------------------------------------------------------------------------------------------\n");

            int choice;
            do
            {
                printf("Select the number of the patient you want to (1-%d): \n", matchCount);
                choice = inputInt("");
            } while (choice < 1 || choice > matchCount);

            select_index = matches[choice - 1];
        }

        visitLogMenu(select_index); // Provide access to visit log for the selected patient
    }
}

void searchPatientByCnic()
{
    int confirm = inputInt("1. Search By CNIC\n2. Go back\nEnter your choice: ");
    while (confirm != 1 && confirm != 2)
    {
        printf("Wrong Choice\n");
        confirm = inputInt("1. Search By CNIC\n2. Go back\nEnter your choice: ");
    }

    char cnic[20];
    printf("Enter CNIC: ");
    inputString(cnic, sizeof(cnic));

    int matches[100];
    int matchCount = 0;

    // Match CNIC or guardian_cnic
    for (int i = 0; i < patient_counter; i++)
    {
        if (patients[i].status == PATIENT_ACTIVE &&
            (strncmp(patients[i].p_cnic, cnic, strlen(cnic)) == 0 ||
             (patients[i].is_minor && strncmp(patients[i].guardian_cnic, cnic, strlen(cnic)) == 0)))
        {
            matches[matchCount++] = i;
        }
    }

    if (matchCount == 0)
    {
        printf("\nNo active patients found matching CNIC '%s'.\n", cnic);
        return;
    }

    int select_index = -1;
    while (1)
    {
        // If only one match found, display it directly
        if (matchCount == 1)
        {
            select_index = matches[0];
            printf(" %-5s | %-5s |%-20s | %-3s | %-10s | %-15s | %-15s | %-15s |\n",
                   "No.", "ID", "Name", "Age", "Gender", "Blood Group", "Disease", "Contact");
            printf(" %-5d | %-5s |%-20s | %-3d | %-10s | %-15s | %-15s | %-15s |\n",
                   1, patients[select_index].patient_id, patients[select_index].p_name, patients[select_index].p_age,
                   patients[select_index].p_gender, patients[select_index].p_blood_group, patients[select_index].p_disease, patients[select_index].p_contact_num);
            printf("---------------------------------------------------------------------------------------------\n");
        }
        else if (matchCount > 1)
        {
            // If multiple matches found, display them and ask user to select one
            printf("\nMultiple patients found with matching CNIC or Guardian CNIC:\n");
            printf("---------------------------------------------------------------------------------------------\n");
            printf(" %-5s | %-5s |%-20s | %-3s | %-10s | %-15s | %-15s | %-15s |\n",
                   "No.", "ID", "Name", "Age", "Gender", "Blood Group", "Disease", "Contact");
            for (int i = 0; i < matchCount; i++)
            {
                int idx = matches[i];
                printf(" %-5d | %-5s |%-20s | %-3d | %-10s | %-15s | %-15s | %-15s |\n",
                       i + 1, patients[idx].patient_id, patients[idx].p_name, patients[idx].p_age,
                       patients[idx].p_gender, patients[idx].p_blood_group, patients[idx].p_disease, patients[idx].p_contact_num);
            }
            printf("---------------------------------------------------------------------------------------------\n");
            int choice;
            do
            {
                printf("Select the number of the patient you want to (1-%d): \n", matchCount);
                choice = inputInt("");
            } while (choice < 1 || choice > matchCount);
            select_index = matches[choice - 1];
        }

        visitLogMenu(select_index); // Provide access to visit log for the selected patient
    }
}

//-----------------------------------Delete Patient--------------------

void deletePatient()
{
    printf("\n=== Delete Patient ===\n");
    int choice;
    do
    {
        printf("1. Delete by ID\n2. Delete by CNIC\n3. Go back\nEnter your choice: ");
        choice = inputInt("");
        if (choice != 1 && choice != 2 && choice != 3)
        {
            printf("Choose Correct Option: \n");
        }
    } while (choice != 1 && choice != 2 && choice != 3);
    if (choice == 1)
    {
        // Existing delete by ID logic this will not delete permenantly, it will just deactivate the patient
        char id[20];
        printf("Enter ID: ");
        inputString(id, sizeof(id));
        int found = 0;
        for (int i = 0; i < patient_counter; i++)
        {
            if (strcmp(patients[i].patient_id, id) == 0 && patients[i].status == PATIENT_ACTIVE)
            {
                printf("\n1. Really want to delete \n\nID: %s \nName : %s\n\n", patients[i].patient_id, patients[i].p_name);
                int confirm2 = inputInt("2. Go back\nEnter your choice: ");
                if (confirm2 != 1)
                {
                    printf("Returning to main menu.\n");
                    return;
                }
                patients[i].status = PATIENT_DEACTIVE;
                printf("Patient with ID %s deleted successfully.\n", id);
                found = 1;
                savePatientsToFile();
            }
        }
        if (!found)
        {
            printf("Patient with ID %s not found.\n", id);
        }
    }
    else if (choice == 2)
    {
        deletePatientByCnic();
    }
    else if (choice == 3)
    {
        printf("Returning to main menu.\n");
        return;
    }
}

// Update Function

void updatePatientById()
{

    int confirm = inputInt("1.Really Want to Update By ID\n2. Go back\nEnter your choice: ");
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }

    char id[20];
    printf("\nEnter ID of patient: ");
    inputString(id, sizeof(id));
    int found = 0;
    // matches each patient with given id if found then  update it
    for (int i = 0; i < patient_counter; i++)
    {
        if (strcmp(patients[i].patient_id, id) == 0 && patients[i].status == PATIENT_ACTIVE)
        {
            found = 1;
            updatePatient(i);

            break; // Update only the first matching record
        }
    }

    if (!found)
    {
        printf("\n No active patient found with ID %s.\n", id);
    }
}

void updatePatientByName()
{

    int confirm = inputInt("1. Really Want to Update By Name\n2. Go back\nEnter your choice: ");
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }

    char name[50];
    printf("\nEnter name of patient: ");
    inputString(name, sizeof(name));

    int matches[100]; // To store indexes of matching patients
    int matchCount = 0;

    // Collect all matching patients (case-insensitive substring match)
    for (int i = 0; i < patient_counter; i++)
    {
        if (patients[i].status == PATIENT_ACTIVE && strncasecmp(patients[i].p_name, name, strlen(name)) == 0)
        {
            matches[matchCount++] = i;
        }
    }

    if (matchCount == 0)
    {
        printf("\n No active patients found matching '%s'.\n", name);
        return;
    }

    int select_index = -1;

    if (matchCount == 1)
    {
        select_index = matches[0]; // Only one match
    }
    else
    {
        printf("\n Multiple patients found with matching name:\n\n");
        printf("---------------------------------------------------------------------------------------------\n");
        printf(" %-5s | %-5s |%-20s | %-3s | %-10s | %-15s | %-15s | %-15s |\n",
               "No.", "ID", "Name", "Age", "Gender", "Blood Group", "Disease", "Contact");

        for (int i = 0; i < matchCount; i++)
        {
            int idx = matches[i];
            printf(" %-5d | %-5s |%-20s | %-3d | %-10s | %-15s | %-15s | %-15s |\n",
                   i + 1, patients[idx].patient_id, patients[idx].p_name, patients[idx].p_age,
                   patients[idx].p_gender, patients[idx].p_blood_group, patients[idx].p_disease, patients[idx].p_contact_num);
        }
        printf("---------------------------------------------------------------------------------------------\n");

        printf("\nSelect the number of the patient you want to update (1-%d): ", matchCount);
        int choice = inputInt("");

        while (choice < 1 || choice > matchCount)
        {
            printf(" Invalid choice. Enter a number between 1 and %d: ", matchCount);
            choice = inputInt("");
        }

        select_index = matches[choice - 1];
    }

    updatePatient(select_index);
}

void updatePatientByCnic()
{
    int confirm = inputInt("1. Really Want to Update By CNIC\n2. Go back\nEnter your choice: ");
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }
    char cnic[20];
    printf("\nEnter CNIC of patient: ");
    inputString(cnic, sizeof(cnic));
    int matches[100];
    int matchCount = 0;
    for (int i = 0; i < patient_counter; i++)
    {
        if (patients[i].status == PATIENT_ACTIVE && strncmp(patients[i].p_cnic, cnic, strlen(cnic)) == 0)
        {
            matches[matchCount++] = i;
        }
    }
    if (matchCount == 0)
    {
        printf("\n No active patients found matching CNIC '%s'.\n", cnic);
        return;
    }
    int select_index = -1;
    if (matchCount == 1)
    {
        select_index = matches[0];
    }
    else
    {
        printf("\n Multiple patients found with matching CNIC:\n\n");
        printf("-----------------------------------------------------------------------------------------------------------------------------\n");
        printf(" %-5s | %-5s |%-20s | %-3s | %-10s | %-15s | %-15s | %-15s |\n\n", "No.", "ID", "Name", "Age", "Gender", "Blood Group", "Disease", "Contact");
        for (int i = 0; i < matchCount; i++)
        {
            int idx = matches[i];
            printf(" %-5d | %-5s |%-20s | %-3d | %-10s | %-15s | %-15s | %-15s |\n",
                   i + 1, patients[idx].patient_id, patients[idx].p_name, patients[idx].p_age,
                   patients[idx].p_gender, patients[idx].p_blood_group, patients[idx].p_disease, patients[idx].p_contact_num);
        }
        printf("-----------------------------------------------------------------------------------------------------------------------------\n");
        printf("\nSelect the number of the patient you want to update (1-%d): ", matchCount);
        int choice = inputInt("");
        while (choice < 1 || choice > matchCount)
        {
            printf(" Invalid choice. Enter a number between 1 and %d: ", matchCount);
            choice = inputInt("");
        }
        select_index = matches[choice - 1];
    }
    updatePatient(select_index);
}

void deletePatientByCnic()
{
    printf("\n=== Delete Patient By CNIC ===\n");
    int confirm;
    do
    {
        confirm = inputInt("1. Proceed to delete by CNIC\n2. Go back\nEnter your choice: ");
        if (confirm != 1 && confirm != 2)
        {
            printf("Choose Correct Option: \n");
        }
    } while (confirm != 1 && confirm != 2);
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }
    char cnic[20];
    printf("Enter CNIC: ");
    inputString(cnic, sizeof(cnic));
    int matches[100];
    int matchCount = 0;
    for (int i = 0; i < patient_counter; i++)
    {
        if (patients[i].status == PATIENT_ACTIVE && strncmp(patients[i].p_cnic, cnic, strlen(cnic)) == 0)
        {
            matches[matchCount++] = i;
        }
    }
    if (matchCount == 0)
    {
        printf("\n No active patients found matching CNIC '%s'.\n", cnic);
        return;
    }
    int select_index = -1;
    if (matchCount == 1)
    {
        select_index = matches[0];
    }
    else
    {
        printf("\n Multiple patients found with matching CNIC:\n\n");
        printf("-----------------------------------------------------------------------------------------------------------------------------\n");
        printf(" %-5s | %-5s |%-20s | %-3s | %-10s | %-15s | %-15s | %-15s |\n\n", "No.", "ID", "Name", "Age", "Gender", "Blood Group", "Disease", "Contact");
        for (int i = 0; i < matchCount; i++)
        {
            int idx = matches[i];
            printf(" %-5d | %-5s |%-20s | %-3d | %-10s | %-15s | %-15s | %-15s |\n",
                   i + 1, patients[idx].patient_id, patients[idx].p_name, patients[idx].p_age,
                   patients[idx].p_gender, patients[idx].p_blood_group, patients[idx].p_disease, patients[idx].p_contact_num);
        }
        printf("-----------------------------------------------------------------------------------------------------------------------------\n");
        printf("\nSelect the number of the patient you want to delete (1-%d): ", matchCount);
        int choice = inputInt("");
        while (choice < 1 || choice > matchCount)
        {
            printf(" Invalid choice. Enter a number between 1 and %d: ", matchCount);
            choice = inputInt("");
        }
        select_index = matches[choice - 1];
    }
    printf("\n1. Really want to delete \n\nID: %s \nName : %s\n\n", patients[select_index].patient_id, patients[select_index].p_name);
    int confirm2 = inputInt("2. Go back\nEnter your choice: ");
    if (confirm2 != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }
    patients[select_index].status = PATIENT_DEACTIVE;
    printf("Patient with CNIC %s deleted successfully.\n", patients[select_index].p_cnic);
    savePatientsToFile();
}

// SECONDARY FUNCTIONS

// --------------------------------------------Display Patients
void displayActivePatient()
{
    FILE *file = fopen("./data/patient.dat", "rb");
    fileCheck(file);
    printf("\n List of All Patients:\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-8s | %-20s | %-3s | %-6s | %-15s | %-15s | %-15s | %-15s | %-6s | %-19s | %-16d |\n", "PID", "Name", "Age", "Gender", "Blood Group", "Disease", "Contact", "CNIC", "Minor", "Registration D&T", "Assigned Doctor ID");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < patient_counter; i++)
    {
        if (patients[i].status == PATIENT_ACTIVE)
        {
            time_t currentTime = time(NULL);
            char reg_time_str[25];
            formatRegistrationTime(currentTime, reg_time_str, sizeof(reg_time_str));
            printf("| %-8s | %-20s | %-3d | %-6s | %-15s | %-15s | %-15s | %-15s | %-6s | %-19s | %-16d |\n",
                   patients[i].patient_id, patients[i].p_name, patients[i].p_age, patients[i].p_gender,
                   patients[i].p_blood_group, patients[i].p_disease, patients[i].p_contact_num,
                   patients[i].is_minor ? (strlen(patients[i].guardian_cnic) ? patients[i].guardian_cnic : "-") : patients[i].p_cnic,
                   patients[i].is_minor ? "Minor" : "Adult", reg_time_str,
                   patients[i].assigned_doctor_id);
        }
    }
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    if (patient_counter == 0)
        printf("No patient records found.\n");
}

void displayDeactivePatient()
{
    FILE *file = fopen("./data/patient.dat", "rb");
    fileCheck(file);
    printf("\n List of Deactive Patients:\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-8s | %-20s | %-3s | %-6s | %-15s | %-15s | %-15s | %-15s | %-6s | %-19s | %-16d |\n", "PID", "Name", "Age", "Gender", "Blood Group", "Disease", "Contact", "CNIC", "Minor", "Registration D&T" , "Assigned Patient ID");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < patient_counter; i++)
    {
        if (patients[i].status == PATIENT_DEACTIVE)
        {
            char reg_time_str[25];
            time_t currentTime = time(NULL);
            formatRegistrationTime(currentTime, reg_time_str, sizeof(reg_time_str));
            printf("| %-8s | %-20s | %-3d | %-6s | %-15s | %-15s | %-15s | %-15s | %-6s | %-19s | %-16d |\n",
                   patients[i].patient_id, patients[i].p_name, patients[i].p_age, patients[i].p_gender,
                   patients[i].p_blood_group, patients[i].p_disease, patients[i].p_contact_num,
                   patients[i].is_minor ? (strlen(patients[i].guardian_cnic) ? patients[i].guardian_cnic : "-") : patients[i].p_cnic,
                   patients[i].is_minor ? "Minor" : "Adult", reg_time_str,
                   patients[i].assigned_doctor_id);
        }
    }
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    if (patient_counter == 0)
        printf("No patient records found.\n");
}
void displayAllPatient()
{
    FILE *file = fopen("./data/patient.dat", "rb");
    fileCheck(file);
    printf("\n List of All Patients:\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-8s | %-20s | %-3s | %-6s | %-15s | %-15s | %-15s | %-15s | %-6s | %-19s | %-16d |\n", "PID", "Name", "Age", "Gender", "Blood Group", "Disease", "Contact", "CNIC", "Minor", "Registration D&T" , "Assigned Doctor ID");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < patient_counter; i++)
    {
        char reg_time_str[25];
        time_t current_time = time(NULL);
        formatRegistrationTime(current_time, reg_time_str, sizeof(reg_time_str));
        printf("| %-8s | %-20s | %-3d | %-6s | %-15s | %-15s | %-15s | %-15s | %-6s | %-19s | %-16d |\n",
               patients[i].patient_id,
               patients[i].p_name,
               patients[i].p_age,
               patients[i].p_gender,
               patients[i].p_blood_group,
               patients[i].p_disease,
               patients[i].p_contact_num,
               patients[i].is_minor ? (strlen(patients[i].guardian_cnic) ? patients[i].guardian_cnic : "-") : patients[i].p_cnic,
               patients[i].is_minor ? "Minor" : "Adult",
               reg_time_str,
            patients[i].assigned_doctor_id);
    }
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    if (patient_counter == 0)
        printf("No patient records found.\n");
}

//----------------------------------savePatientsToFile

void savePatientsToFile()
{
    FILE *file = fopen("./data/patient.dat", "wb");
    if (!file)
    {
        printf("Error: Could not open patient.dat for writing\n");
        return;
    }

    for (int i = 0; i < patient_counter; i++)
    {
        if (fwrite(&patients[i], sizeof(Patient), 1, file) != 1)
        {
            printf("Error: Failed to write patient data to file\n");
            fclose(file);
            return;
        }
    }
    fclose(file);

    // Also save to CSV for backup/export
    FILE *file2 = fopen("./data/patient.csv", "w");
    if (!file2)
    {
        printf("Error: Could not open patient.csv for writing\n");
        return;
    }

    fprintf(file2, "ID,Name,Age,Gender,Disease,Contact,CNIC,GuardianCNIC,BloodGroup,IsMinor,RegistrationTime,Status,patient.assigned_doctor_id\n");

    for (int i = 0; i < patient_counter; i++)
    {
        fprintf(file2, "%s,%s,%d,%s,%s,\"%s\",\"%s\",\"%s\",%s,%d,%lld,%d,%d\n",
                patients[i].patient_id,
                patients[i].p_name,
                patients[i].p_age,
                patients[i].p_gender,
                patients[i].p_disease,
                patients[i].p_contact_num,
                patients[i].p_cnic,
                patients[i].guardian_cnic,
                patients[i].p_blood_group,
                patients[i].is_minor,
                patients[i].registration_time,
                patients[i].status,
                patients[i].assigned_doctor_id);
    }
    fclose(file2);
}
void displayPatient()
{
    int choice;
    do
    {
        printf("\n=== Display Patients ===\n");
        printf("1. Display Active Patients\n");
        printf("2. Display Deactive Patients\n");
        printf("3. Display All Patients\n");
        printf("4. Go Back\n");
        choice = inputInt("Enter your choice: ");

        switch (choice)
        {
        case 1:
            displayActivePatient();
            break;
        case 2:
            displayDeactivePatient();
            break;
        case 3:
            displayAllPatient();
            break;
        case 4:
            return; // Go back to the main menu
        default:
            printf("Invalid choice! Please enter a number between 1 and 4.\n");
        }
    } while (choice != 4);
}
// ----------------------------------------Display Menu Function
void displayPatientMenu(void)
{
    printf("\n=== Hospital Management System ===\n");
    printf("1. Add a Patient\n");
    printf("2. Display All Patients\n");
    printf("3. Search for a Patient\n");
    printf("4. Update a Patient\n");
    printf("5. Delete a Patient\n");
    printf("6. Exit\n");
    printf("==============================\n");
}

//---------------------------------------------Load Patient
void loadPatientFromFile(void)
{
    FILE *fp = fopen("./data/patient.dat", "rb");
    if (!fp)
    {
        printf("No existing patient data file found. Starting with empty database.\n");
        patient_counter = 0;
        return;
    }

    patient_counter = 0; // Reset counter before loading
    while (fread(&patients[patient_counter], sizeof(Patient), 1, fp) != 0 && patient_counter < MAX_PATIENTS)
    {
        patient_counter++;
    }
    fclose(fp);
}

int inputValidatedDisease(char *disease, int size)
{
    char input[60];
    int valid = 0;
    while (!valid)
    {
        printf("Enter disease (alphabets and spaces only): ");
        inputString(input, sizeof(input));
        valid = 1;
        for (int i = 0; input[i] != '\0'; i++)
        {
            if (!isalpha((unsigned char)input[i]) && input[i] != ' ')
            {
                valid = 0;
                break;
            }
        }
        if (!valid)
        {
            printf("Invalid disease. Please use alphabets and spaces only.\n");
        }
    }
    strncpy(disease, input, size);
    return 1;
}

// Function to handle patient action menu (add visit, display history, etc.)

void updatePatient(int select_index)
{
    int updated = 0;

    while (1)
    {
        printf("\n--- Existing Info of patient with ID %s ---\n", patients[select_index].patient_id);
        printf("1. Name           : %s\n", patients[select_index].p_name);
        printf("2. Age            : %d\n", patients[select_index].p_age);
        printf("3. Gender         : %s\n", patients[select_index].p_gender);
        printf("4. Disease        : %s\n", patients[select_index].p_disease);
        printf("5. Contact Number : %s\n", patients[select_index].p_contact_num);
        printf("6. Blood Group    : %s\n", patients[select_index].p_blood_group);
        printf("7. CNIC          : %s\n", patients[select_index].p_cnic);
        printf("8. << Go Back (Finish Updating)\n\n");

        int choice = inputInt("Which field do you want to change (1-8): ");
        while (choice < 1 || choice > 8)
        {
            printf("Invalid choice! Please enter a valid choice between 1 and 8.\n");
            choice = inputInt("Enter a choice: ");
        }

        if (choice == 8)
        {
            break; // Exit the loop to final save/cancel decision
        }

        const char *fieldName[] = {"Name", "Age", "Gender", "Disease", "Contact Number", "Blood Group", "CNIC"};
        printf("\nYou chose to update *%s*.\n", fieldName[choice - 1]);
        printf("1. Proceed to update\n");
        printf("2. Go back to menu\n");
        int sub_choice = inputInt("Enter your choice: ");

        if (sub_choice != 1)
        {
            continue; // Go back to field menu
        }

        // Update selected field
        switch (choice)
        {
        case 1:
            inputValidatedName(patients[select_index].p_name, sizeof(patients[select_index].p_name));
            break;
        case 2:
            patients[select_index].p_age = inputValidatedAge();
            break;
        case 3:
            inputValidatedGender(patients[select_index].p_gender, sizeof(patients[select_index].p_gender));
            break;
        case 4:
            inputValidatedDisease(patients[select_index].p_disease, sizeof(patients[select_index].p_disease));
            break;
        case 5:
            inputValidatedContact(patients[select_index].p_contact_num, sizeof(patients[select_index].p_contact_num));
            break;
        case 6:
            inputValidatedBloodGroup(patients[select_index].p_blood_group, sizeof(patients[select_index].p_blood_group));
            break;
        case 7:
            inputValidatedCNIC(patients[select_index].p_cnic, sizeof(patients[select_index].p_cnic));
            break;
        }

        updated = 1;

        printf("\nDo you want to update another field?\n");
        printf("1. Yes\n");
        printf("2. No (Continue to Save)\n");
        int more = inputInt("Enter your choice: ");
        if (more != 1)
        {
            break;
        }
    }

    if (updated)
    {
        printf("\nDo you want to save the changes?\n");
        printf("1. Save and Exit\n");
        printf("2. Cancel without Saving\n");
        int sub_choice2 = inputInt("Enter your choice: ");

        while (sub_choice2 != 1 && sub_choice2 != 2)
        {
            printf("Invalid option. Please try again.\n");
            sub_choice2 = inputInt("Enter your choice: ");
        }

        if (sub_choice2 == 1)
        {
            savePatientsToFile();
            printf("\n Patient record updated and saved successfully.\n");
        }
        else
        {
            printf("\n Update cancelled. No changes were saved.\n");
        }
    }
    else
    {
        printf("\n No changes were made to the patient record.\n");
    }
}
