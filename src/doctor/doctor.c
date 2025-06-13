#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "src/doctor/doctor.h"
#include "src/common/common.h"
#include "src/id_manager/id_manager.h"
#include "src/VisitLog/visitlog.h"
#include "doctor.h"
#include "patient.h"

// Core variables
Doctor doctors[MAX_DOCTORS];
int doctor_counter = 0;

void doctorModule()
{
    loadDoctorsFromFile();
    loadIDManager();
    int choice;
    do
    {
        displayDoctorMenu();
        choice = inputInt("Enter your choice: ");

        switch (choice)
        {
        case 1:
            addDoctor();
            break;
        case 2:
            displayDoctors();
            break;
        case 3:
            searchDoctor();
            break;
        case 4:
            deleteDoctor();
            break;
        case 5:
            updateDoctorMenu();

            break;

        case 6:
            mainFunction();
            break;
        default:
            printf("Invalid choice! Please enter a number between 1 and 6.\n");
        }
    } while (choice != 6);
}

void updateDoctorMenu()
{

    int update_choice;
    printf("\nUpdate Doctor by:\n1. ID\n2. Name\n3. CNIC\n4.<<<<<Go Back\n");
    update_choice = inputInt("Enter choice: ");
    while (update_choice != 1 && update_choice != 2 && update_choice != 3 && update_choice != 4)
    {
        printf("Please choose correct option!");
        printf("1. ID\n2. Name\n3. CNIC\n4.<<<<<Go Back\n");
        update_choice = inputInt("Enter choice: ");
    }
    if (update_choice == 1)
        updateDoctorById();
    else if (update_choice == 2)
        updateDoctorByName();
    else if (update_choice == 3)
        updateDoctorByCnic();
    else if (update_choice == 4)
        return;
    else
        printf("Invalid update choice!\n");
}
void addDoctor()
{
    if (doctor_counter >= MAX_DOCTORS)
    {
        printf("Maximum number of doctors reached.\n");
        return;
    }

    printf("\n=== Add New Doctor ===\n");
    printf("Is this your first admission? (Y/N): ");
    char is_first_admission[10];
    inputString(is_first_admission, sizeof(is_first_admission));

    while (!(strcasecmp(is_first_admission, "y") == 0 || strcasecmp(is_first_admission, "yes") == 0 ||
             strcasecmp(is_first_admission, "n") == 0 || strcasecmp(is_first_admission, "no") == 0))
    {
        printf("Select Correct Option: (Y/N) ");
        inputString(is_first_admission, sizeof(is_first_admission));
    }

    if (strcasecmp(is_first_admission, "n") == 0 || strcasecmp(is_first_admission, "no") == 0)
    {
        // Not first admission - check existing CNIC
        char cnic[15];
        printf("Enter your CNIC: ");
        inputValidatedCNIC(cnic, sizeof(cnic));

        // Search for the CNIC in both active and deactivated doctors
        int found_index = -1;
        for (int i = 0; i < doctor_counter; i++)
        {
            if (strcmp(doctors[i].d_cnic, cnic) == 0)
            {
                found_index = i;
                break;
            }
        }

        if (found_index != -1)
        {
            if (doctors[found_index].status == DOCTOR_ACTIVE)
            {
                printf("\nWelcome back Dr. %s! Your account is already active.\n", doctors[found_index].d_name);
                return;
            }
            else if (doctors[found_index].status == DOCTOR_BLOCK)
            {
                printf("\nYour account has been blocked due to multiple failed login attempts.\n");
                printf("Please contact the administrator for assistance.\n");
                return;
            }
            else // DOCTOR_DEACTIVE
            {
                printf("\nFound your deactivated account. Would you like to reactivate it? (Y/N): ");
                char reactivate_choice[10];
                inputString(reactivate_choice, sizeof(reactivate_choice));

                if (strcasecmp(reactivate_choice, "y") == 0 || strcasecmp(reactivate_choice, "yes") == 0)
                {
                    int attempts = 0;
                    char password[MAX_PASSWORD_LENGTH];
                    int password_correct = 0;

                    while (attempts < MAX_LOGIN_ATTEMPTS && password_correct == 0)
                    {
                        printf("Enter your password: ");
                        inputString(password, sizeof(password));

                        if (strcmp(doctors[found_index].password, password) == 0)
                        {
                            password_correct = 1;
                            doctors[found_index].status = DOCTOR_ACTIVE;
                            doctors[found_index].failed_login_attempts = 0;
                            saveDoctorsToFile();
                            printf("\nAccount reactivated successfully! Welcome back Dr. %s.\n", doctors[found_index].d_name);
                            return;
                        }
                        else
                        {
                            attempts++;
                            if (attempts < MAX_LOGIN_ATTEMPTS)
                            {
                                printf("Incorrect password. %d attempts remaining.\n", MAX_LOGIN_ATTEMPTS - attempts);
                            }
                        }
                    }

                    if (password_correct == 0)
                    {
                        doctors[found_index].status = DOCTOR_BLOCK;
                        doctors[found_index].failed_login_attempts = MAX_LOGIN_ATTEMPTS;
                        saveDoctorsToFile();
                        printf("\nToo many failed attempts. Your account has been blocked.\n");
                        printf("Please contact the administrator for assistance.\n");
                        return;
                    }
                }
                else
                {
                    printf("\nAccount reactivation cancelled.\n");
                    return;
                }
            }
        }
        else
        {
            printf("\nNo account found with this CNIC. Please proceed with new registration.\n");
        }
    }
    else if (strcasecmp(is_first_admission, "y") == 0 || strcasecmp(is_first_admission, "yes") == 0)
    {
        // First admission or new registration
        Doctor new_doctor;
        memset(&new_doctor, 0, sizeof(Doctor));

        // Get doctor information
        inputValidatedName(new_doctor.d_name, sizeof(new_doctor.d_name));
        new_doctor.d_age = inputValidatedAge();
        inputValidatedGender(new_doctor.d_gender, sizeof(new_doctor.d_gender));
        inputValidatedContact(new_doctor.d_phone, sizeof(new_doctor.d_phone));

        printf("Enter Specialization: ");
        inputString(new_doctor.d_specialization, sizeof(new_doctor.d_specialization));
        inputValidatedCNIC(new_doctor.d_cnic, sizeof(new_doctor.d_cnic));

        // Check for CNIC uniqueness

        for (int i = 0; i < doctor_counter; i++)
        {

            if (strcmp(doctors[i].d_cnic, new_doctor.d_cnic) == 0)
            {

                if (doctors[i].status == DOCTOR_ACTIVE)
                {
                    printf("Error: A doctor with this CNIC is already registered and active.\n");
                    printf("Enter Correct CNIC: ");

                    inputValidatedCNIC(new_doctor.d_cnic, sizeof(new_doctor.d_cnic));

                    break;
                    ;
                }
                else if (doctors[i].status == DOCTOR_DEACTIVE)
                {
                    printf("This CNIC is already assigned to \nName : %s \nID :%s : Is it you? ", doctors[i].d_name, doctors[i].d_id);
                    char choice[5];
                    inputString(choice, sizeof(choice));
                    while (!(strcmp(choice, "y") == 0 || strcmp(choice, "yes") == 0) || (strcmp(choice, "n") == 0 || strcmp(choice, "no") == 0))
                    {
                        inputString(choice, sizeof(choice));
                        printf("\nPlease Choose Correct Option! : (Y/N)");
                    }

                    if ((strcmp(choice, "y") == 0 || strcmp(choice, "yes") == 0))
                    {
                        /* code */

                        printf("\nFound your deactivated account. Would you like to reactivate it? (Y/N): ");
                        char reactivate_choice[10];
                        inputString(reactivate_choice, sizeof(reactivate_choice));

                        if (strcasecmp(reactivate_choice, "y") == 0 || strcasecmp(reactivate_choice, "yes") == 0)
                        {
                            int attempts = 0;
                            char password[MAX_PASSWORD_LENGTH];
                            int password_correct = 0;

                            while (attempts < MAX_LOGIN_ATTEMPTS && password_correct == 0)
                            {
                                printf("Enter your password: ");
                                inputString(password, sizeof(password));

                                if (strcmp(doctors[i].password, password) == 0)
                                {
                                    password_correct = 1;
                                    doctors[i].status = DOCTOR_ACTIVE;
                                    doctors[i].failed_login_attempts = 0;
                                    saveDoctorsToFile();
                                    printf("\nAccount reactivated successfully! Welcome back Dr. %s.\n", doctors[i].d_name);
                                    return;
                                }
                                else if (doctors[i].status == DOCTOR_BLOCK)
                                {
                                    printf("Error: This CNIC is associated with a blocked account.\n");
                                    printf("Please contact the administrator for assistance.\n");
                                    return;
                                }
                                else
                                {
                                    attempts++;
                                    if (attempts < MAX_LOGIN_ATTEMPTS)
                                    {
                                        printf("Incorrect password. %d attempts remaining.\n", MAX_LOGIN_ATTEMPTS - attempts);
                                    }
                                }
                            }

                            if (password_correct == 0)
                            {
                                doctors[i].status = DOCTOR_BLOCK;
                                doctors[i].failed_login_attempts = MAX_LOGIN_ATTEMPTS;
                                saveDoctorsToFile();
                                printf("\nToo many failed attempts. Your account has been blocked.\n");
                                printf("Please contact the administrator for assistance.\n");
                                return;
                            }
                        }
                        else if ((strcmp(choice, "n") == 0 || strcmp(choice, "no") == 0))
                        {
                            printf("Enter Corerct CNIC: ");
                            inputValidatedCNIC(new_doctor.d_cnic, sizeof(new_doctor.d_cnic));
                            break;
                        }

                        else
                        {
                            printf("\nAccount reactivation cancelled.\n");
                            return;
                        }
                    }
                }
            }
        }

        // Set password for new doctor
        printf("Set password for the doctor: ");
        inputString(new_doctor.password, sizeof(new_doctor.password));

        // Generate doctor ID
        snprintf(new_doctor.d_id, sizeof(new_doctor.d_id), "D%05d", doctor_counter + 1);

        new_doctor.registration_time = time(NULL);
        new_doctor.status = DOCTOR_ACTIVE;
        new_doctor.failed_login_attempts = 0;

        doctors[doctor_counter++] = new_doctor;
        saveDoctorsToFile();
        printf("Doctor added successfully! ID: %s\n", new_doctor.d_id);
    }
}

void searchDoctor()
{
    int choice;
    do
    {
        printf("\n=== Search Doctor ===\n");
        printf("1. Search by ID\n");
        printf("2. Search by Name\n");
        printf("3. Search by Specialization\n");
        printf("4. Search by CNIC\n");
        printf("0. Back to Main Menu\n");

        choice = inputInt("Enter your choice:");
        switch (choice)
        {
        case 1:
            searchDoctorById();
            break;
        case 2:
            searchDoctorByName();
            break;
        case 3:
            searchDoctorBySpecialization();
            break;
        case 4:
            searchDoctorByCnic();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
        }
    } while (choice != 0);
}

void searchDoctorById()
{
    char id[10];
    printf("Enter Doctor ID: ");
    inputString(id, sizeof(id));

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("| %-8s | %-20s | %-15s | %-15s | %-20s | %-20s |\n",
           "ID", "Name", "CNIC", "Phone", "Specialization", "Registration Time");
    printf("--------------------------------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < doctor_counter; i++)
    {
        if (strcmp(doctors[i].d_id, id) == 0 && doctors[i].status == DOCTOR_ACTIVE)
        {
            char reg_time_str[25];
            formatRegistrationTime(doctors[i].registration_time, reg_time_str, sizeof(reg_time_str));

            printf("| %-8s | %-20s | %-15s | %-15s | %-20s | %-20s |\n",
                   doctors[i].d_id,
                   doctors[i].d_name,
                   doctors[i].d_cnic,
                   doctors[i].d_phone,
                   doctors[i].d_specialization,
                   reg_time_str);
            found = 1;
            break;
        }
    }
    printf("--------------------------------------------------------------------------------------------------------\n");
    if (!found)
    {
        printf("Doctor with ID %s not found.\n", id);
    }
}

void searchDoctorByName()
{
    char name[50];
    printf("Enter Doctor Name: ");
    inputString(name, sizeof(name));

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("| %-8s | %-20s | %-15s | %-15s | %-20s | %-20s |\n",
           "ID", "Name", "CNIC", "Phone", "Specialization", "Registration Time");
    printf("--------------------------------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < doctor_counter; i++)
    {
        if (doctors[i].status == DOCTOR_ACTIVE && strncasecmp(doctors[i].d_name, name, strlen(name)) == 0)
        {
            char reg_time_str[25];
            formatRegistrationTime(doctors[i].registration_time, reg_time_str, sizeof(reg_time_str));

            printf("| %-8s | %-20s | %-15s | %-15s | %-20s | %-20s |\n",
                   doctors[i].d_id,
                   doctors[i].d_name,
                   doctors[i].d_cnic,
                   doctors[i].d_phone,
                   doctors[i].d_specialization,
                   reg_time_str);
            found = 1;
        }
    }
    printf("--------------------------------------------------------------------------------------------------------\n");
    if (!found)
    {
        printf("No doctors found matching '%s'.\n", name);
    }
}

void searchDoctorBySpecialization()
{
    char specialization[50];
    int found = 0;
    printf("Enter Specialization : ");
    inputString(specialization, sizeof(specialization));

    printf("-----------------------------------------------------------------------------------------------\n");
    printf("| %-5s | %-20s | %-3s | %-6s | %-15s | %-15s | %-15s | %-15s |\n",
           "ID", "Name", "Age", "Gender", "Specialization", "Contact", "CNIC", "Registration D&T");
    printf("-----------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < doctor_counter; i++)
    {
        if (strncasecmp(doctors[i].d_specialization, specialization, strlen(specialization)) == 0 && doctors[i].status == DOCTOR_ACTIVE)
        {
            char reg_time_str[25];
            formatRegistrationTime(doctors[i].registration_time, reg_time_str, sizeof(reg_time_str));

            printf("| %-5s | %-20s | %-3d | %-6s | %-15s | %-15s | %-15s | %-15s |\n",
                   doctors[i].d_id, doctors[i].d_name, doctors[i].d_age, doctors[i].d_gender,
                   doctors[i].d_specialization, doctors[i].d_phone, doctors[i].d_cnic, reg_time_str);
            found = 1;
        }
    }
    printf("-----------------------------------------------------------------------------------------------\n");

    if (!found)
    {
        printf("Doctor With Specialization %s Not Found!.\n", specialization);
    }
}

void searchDoctorByCnic()
{
    char cnic[15];
    printf("Enter Doctor CNIC: ");
    inputString(cnic, sizeof(cnic));

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("| %-8s | %-20s | %-15s | %-15s | %-20s | %-20s |\n",
           "ID", "Name", "CNIC", "Phone", "Specialization", "Registration Time");
    printf("--------------------------------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < doctor_counter; i++)
    {
        if (doctors[i].status == DOCTOR_ACTIVE && strncmp(doctors[i].d_cnic, cnic, strlen(cnic)) == 0)
        {
            char reg_time_str[25];
            formatRegistrationTime(doctors[i].registration_time, reg_time_str, sizeof(reg_time_str));

            printf("| %-8s | %-20s | %-15s | %-15s | %-20s | %-20s |\n",
                   doctors[i].d_id,
                   doctors[i].d_name,
                   doctors[i].d_cnic,
                   doctors[i].d_phone,
                   doctors[i].d_specialization,
                   reg_time_str);
            found = 1;
        }
    }
    printf("--------------------------------------------------------------------------------------------------------\n");
    if (!found)
    {
        printf("No doctors found with CNIC '%s'.\n", cnic);
    }
}

void deleteDoctor()
{
    printf("\n=== Delete Doctor ===\n");
    int choice;
    do
    {
        printf("1. Delete by ID\n");
        printf("2. Delete by CNIC\n");
        printf("3. Go back\n");
        printf("Enter your choice: ");
        choice = inputInt("");
        if (choice != 1 && choice != 2 && choice != 3)
        {
            printf("Choose Correct Option: \n");
        }
    } while (choice != 1 && choice != 2 && choice != 3);

    if (choice == 1)
    {
        deleteDoctorById();
    }
    else if (choice == 2)
    {
        deleteDoctorByCnic();
    }
    else if (choice == 3)
    {
        printf("Returning to main menu.\n");
        return;
    }
}

void deleteDoctorById()
{
    int confirm = inputInt("1. Proceed to delete by ID\n2. Go back\nEnter your choice: ");
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }

    char id[20];
    printf("Enter ID: ");
    inputString(id, sizeof(id));
    int found = 0;
    for (int i = 0; i < doctor_counter; i++)
    {
        if (strcmp(doctors[i].d_id, id) == 0 && doctors[i].status == DOCTOR_ACTIVE)
        {
            printf("\n1. Really want to delete \n\nID: %s \nName : %s\n\n", doctors[i].d_id, doctors[i].d_name);
            int confirm2 = inputInt("2. Go back\nEnter your choice: ");
            if (confirm2 != 1)
            {
                printf("Returning to main menu.\n");
                return;
            }
            doctors[i].status = DOCTOR_DEACTIVE;
            printf("Doctor with ID %s deleted successfully.\n", id);
            found = 1;
            saveDoctorsToFile();
        }
    }
    if (!found)
    {
        printf("Doctor with ID %s not found.\n", id);
    }
}

void deleteDoctorByCnic()
{
    printf("\n=== Delete Doctor By CNIC ===\n");
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

    for (int i = 0; i < doctor_counter; i++)
    {
        if (doctors[i].status == DOCTOR_ACTIVE && strncmp(doctors[i].d_cnic, cnic, strlen(cnic)) == 0)
        {
            matches[matchCount++] = i;
        }
    }

    if (matchCount == 0)
    {
        printf("\nNo active doctors found matching CNIC '%s'.\n", cnic);
        return;
    }

    int select_index = -1;
    if (matchCount == 1)
    {
        select_index = matches[0];
    }
    else
    {
        printf("\nMultiple doctors found with matching CNIC:\n\n");
        printf("-----------------------------------------------------------------------------------------------\n");
        printf("| %-5s | %-20s | %-3s | %-6s | %-15s | %-15s | %-15s |\n", "ID", "Name", "Age", "Gender", "Specialization", "Contact", "CNIC");

        for (int i = 0; i < matchCount; i++)
        {
            int idx = matches[i];
            printf("| %-5s | %-20s | %-3d | %-6s | %-15s | %-15s | %-15s |\n",
                   doctors[idx].d_id, doctors[idx].d_name, doctors[idx].d_age,
                   doctors[idx].d_gender, doctors[idx].d_specialization, doctors[idx].d_phone, doctors[idx].d_cnic);
        }
        printf("-----------------------------------------------------------------------------------------------\n");

        printf("\nSelect the number of the doctor you want to delete (1-%d): ", matchCount);
        int choice = inputInt("");
        while (choice < 1 || choice > matchCount)
        {
            printf("Invalid choice. Enter a number between 1 and %d: ", matchCount);
            choice = inputInt("");
        }
        select_index = matches[choice - 1];
    }

    printf("\n1. Really want to delete \n\nID: %s \nName : %s\n\n", doctors[select_index].d_id, doctors[select_index].d_name);
    int confirm2 = inputInt("2. Go back\nEnter your choice: ");
    if (confirm2 != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }

    doctors[select_index].status = DOCTOR_DEACTIVE;
    printf("Doctor with CNIC %s deleted successfully.\n", doctors[select_index].d_cnic);
    saveDoctorsToFile();
}

// File handling
void loadDoctorsFromFile()
{
    FILE *file = fopen("./data/doctor.dat", "rb");
    if (!file)
    {
        printf("No existing doctor data file found. Starting with empty database.\n");
        doctor_counter = 0;
        return;
    }

    doctor_counter = 0; // Reset counter before loading
    while (fread(&doctors[doctor_counter], sizeof(Doctor), 1, file) != 0 && doctor_counter < MAX_DOCTORS)
    {
        doctor_counter++;
    }
    fclose(file);
}

void saveDoctorsToFile()
{
    FILE *file = fopen("./data/doctor.dat", "wb");
    if (!file)
    {
        printf("Error: Could not open doctor.dat for writing\n");
        return;
    }

    for (int i = 0; i < doctor_counter; i++)
    {
        if (fwrite(&doctors[i], sizeof(Doctor), 1, file) != 1)
        {
            printf("Error: Failed to write doctor data to file\n");
            fclose(file);
            return;
        }
    }
    fclose(file);

    FILE *file2 = fopen("./data/doctor.csv", "w");
    if (!file2)
    {
        printf("Error: Could not open doctor.csv for writing\n");
        return;
    }

    fprintf(file2, "ID,Name,Age,Gender,Specialization,Phone,CNIC,RegistrationTime,Status\n");

    for (int i = 0; i < doctor_counter; i++)
    {
        fprintf(file2, "%s,%s,%d,%s,%s,\"%s\",\"%s\",%lld,%d\n",
                doctors[i].d_id,
                doctors[i].d_name,
                doctors[i].d_age,
                doctors[i].d_gender,
                doctors[i].d_specialization,
                doctors[i].d_phone,
                doctors[i].d_cnic,
                doctors[i].registration_time,
                doctors[i].status);
    }
    fclose(file2);
}

// Others

void displayDoctorMenu()
{
    printf("\n=== Doctor Module ===\n");
    printf("1. Add a Doctor\n");
    printf("2. Display All Doctors\n");
    printf("3. Search for a Doctor\n");
    printf("4. Delete a Doctor\n");
    printf("5. Update a Doctor\n");
    printf("6. Exit\n");
    printf("==============================\n");
}

void displayDoctors()
{
    int choice;
    do
    {
        printf("\n=== Display Doctors ===\n");
        printf("1. Display Active Doctors\n");
        printf("2. Display Unavailable Doctors\n");
        printf("3. Display All Doctors\n");
        printf("4. Go Back\n");
        choice = inputInt("Enter your choice: ");

        switch (choice)
        {
        case 1:
            displayActiveDoctors();
            break;
        case 2:
            displayDeactiveDoctors();
            break;
        case 3:
            displayAllDoctors();
            break;
        case 4:
            return; // Go back to the main menu
        default:
            printf("Invalid choice! Please enter a number between 1 and 4.\n");
        }
    } while (choice != 4);
}

void displayActiveDoctors()
{
    printf("\n=== Active Doctors ===\n");
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-8s | %-20s | %-3s | %-6s | %-15s | %-15s | %-20s | %-20s |\n",
           "ID", "Name", "Age", "Gender", "CNIC", "Phone", "Specialization", "Registration Time");
    printf("------------------------------------------------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < doctor_counter; i++)
    {
        if (doctors[i].status == DOCTOR_ACTIVE)
        {
            char reg_time_str[25];
            formatRegistrationTime(doctors[i].registration_time, reg_time_str, sizeof(reg_time_str));

            printf("| %-8s | %-20s | %-3d | %-6s | %-15s | %-15s | %-20s | %-20s |\n",
                   doctors[i].d_id,
                   doctors[i].d_name,
                   doctors[i].d_age,
                   doctors[i].d_gender,
                   doctors[i].d_cnic,
                   doctors[i].d_phone,
                   doctors[i].d_specialization,
                   reg_time_str);
            found = 1;
        }
    }
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    if (!found)
    {
        printf("No active doctors found.\n");
    }
}

void displayDeactiveDoctors()
{
    printf("\n=== Deactive Doctors ===\n");
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-8s | %-20s | %-3s | %-6s | %-15s | %-15s | %-20s | %-20s |\n",
           "ID", "Name", "Age", "Gender", "CNIC", "Phone", "Specialization", "Registration Time");
    printf("------------------------------------------------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < doctor_counter; i++)
    {
        if (doctors[i].status == DOCTOR_DEACTIVE)
        {
            char reg_time_str[25];
            formatRegistrationTime(doctors[i].registration_time, reg_time_str, sizeof(reg_time_str));

            printf("| %-8s | %-20s | %-3d | %-6s | %-15s | %-15s | %-20s | %-20s |\n",
                   doctors[i].d_id,
                   doctors[i].d_name,
                   doctors[i].d_age,
                   doctors[i].d_gender,
                   doctors[i].d_cnic,
                   doctors[i].d_phone,
                   doctors[i].d_specialization,
                   reg_time_str);
            found = 1;
        }
    }
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    if (!found)
    {
        printf("No deactive doctors found.\n");
    }
}

void displayAllDoctors()
{
    printf("\n=== All Doctors ===\n");
    printf("----------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-8s | %-20s | %-3s | %-6s | %-15s | %-15s | %-20s | %-20s | %-10s |\n",
           "ID", "Name", "Age", "Gender", "CNIC", "Phone", "Specialization", "Registration Time", "Status");
    printf("----------------------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < doctor_counter; i++)
    {
        char reg_time_str[25];
        formatRegistrationTime(doctors[i].registration_time, reg_time_str, sizeof(reg_time_str));
        const char *status_str = doctors[i].status == DOCTOR_ACTIVE ? "Active" : doctors[i].status == DOCTOR_DEACTIVE ? "Deactive"
                                                                                                                      : "Blocked";

        printf("| %-8s | %-20s | %-3d | %-6s | %-15s | %-15s | %-20s | %-20s | %-10s |\n",
               doctors[i].d_id,
               doctors[i].d_name,
               doctors[i].d_age,
               doctors[i].d_gender,
               doctors[i].d_cnic,
               doctors[i].d_phone,
               doctors[i].d_specialization,
               reg_time_str,
               status_str);
    }
    printf("----------------------------------------------------------------------------------------------------------------------------\n");
    if (doctor_counter == 0)
    {
        printf("No doctor records found.\n");
    }
}

void updateDoctorById()
{
    int confirm = inputInt("1.Really Want to Update By ID\n2. Go back\nEnter your choice: ");
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }

    char id[20];
    printf("\nEnter ID of doctor: ");
    inputString(id, sizeof(id));
    int found = 0;
    for (int i = 0; i < doctor_counter; i++)
    {
        if (strcmp(doctors[i].d_id, id) == 0 && doctors[i].status == DOCTOR_ACTIVE)
        {
            found = 1;
            updateDoctorHelper(i);
            break;
        }
    }

    if (!found)
    {
        printf("\nNo active doctor found with ID %s.\n", id);
    }
}

void updateDoctorByName()
{
    int confirm = inputInt("1. Really Want to Update By Name\n2. Go back\nEnter your choice: ");
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }

    char name[50];
    printf("\nEnter name of doctor: ");
    inputString(name, sizeof(name));

    int matches[100];
    int matchCount = 0;

    for (int i = 0; i < doctor_counter; i++)
    {
        if (doctors[i].status == DOCTOR_ACTIVE && strncasecmp(doctors[i].d_name, name, strlen(name)) == 0)
        {
            matches[matchCount++] = i;
        }
    }

    if (matchCount == 0)
    {
        printf("\nNo active doctors found matching '%s'.\n", name);
        return;
    }

    int select_index = -1;

    if (matchCount == 1)
    {
        select_index = matches[0];
    }
    else
    {
        printf("\nMultiple doctors found with matching name:\n\n");
        printf("-----------------------------------------------------------------------------------------------\n");
        printf("| %-5s | %-20s | %-3s | %-6s | %-15s | %-15s | %-15s | %-15s |\n", "ID", "Name", "Age", "Gender", "Specialization", "Contact", "CNIC", "Registration D&T");

        for (int i = 0; i < matchCount; i++)
        {
            int idx = matches[i];
            char reg_time_str[25];
            formatRegistrationTime(doctors[idx].registration_time, reg_time_str, sizeof(reg_time_str));

            printf("| %-5s | %-20s | %-3d | %-6s | %-15s | %-15s | %-15s | %-15s |\n",
                   doctors[idx].d_id, doctors[idx].d_name, doctors[idx].d_age,
                   doctors[idx].d_gender, doctors[idx].d_specialization, doctors[idx].d_phone, doctors[idx].d_cnic, reg_time_str);
        }
        printf("-----------------------------------------------------------------------------------------------\n");

        printf("\nSelect the number of the doctor you want to update (1-%d): ", matchCount);
        int choice = inputInt("");
        while (choice < 1 || choice > matchCount)
        {
            printf("Invalid choice. Enter a number between 1 and %d: ", matchCount);
            choice = inputInt("");
        }
        select_index = matches[choice - 1];
    }

    updateDoctorHelper(select_index);
}

void updateDoctorByCnic()
{
    int confirm = inputInt("1. Really Want to Update By CNIC\n2. Go back\nEnter your choice: ");
    if (confirm != 1)
    {
        printf("Returning to main menu.\n");
        return;
    }

    char cnic[20];
    printf("\nEnter CNIC of doctor: ");
    inputString(cnic, sizeof(cnic));

    int matches[100];
    int matchCount = 0;

    for (int i = 0; i < doctor_counter; i++)
    {
        if (doctors[i].status == DOCTOR_ACTIVE && strncmp(doctors[i].d_cnic, cnic, strlen(cnic)) == 0)
        {
            matches[matchCount++] = i;
        }
    }

    if (matchCount == 0)
    {
        printf("\nNo active doctors found matching CNIC '%s'.\n", cnic);
        return;
    }

    int select_index = -1;

    if (matchCount == 1)
    {
        select_index = matches[0];
    }
    else
    {
        printf("\nMultiple doctors found with matching CNIC:\n\n");
        printf("-----------------------------------------------------------------------------------------------\n");
        printf("| %-5s | %-20s | %-3s | %-6s | %-15s | %-15s | %-15s | %-15s |\n", "ID", "Name", "Age", "Gender", "Specialization", "Contact", "CNIC", "Registration D&T");

        for (int i = 0; i < matchCount; i++)
        {
            int idx = matches[i];
            char reg_time_str[25];
            formatRegistrationTime(doctors[idx].registration_time, reg_time_str, sizeof(reg_time_str));

            printf("| %-5s | %-20s | %-3d | %-6s | %-15s | %-15s | %-15s | %-15s |\n",
                   doctors[idx].d_id, doctors[idx].d_name, doctors[idx].d_age,
                   doctors[idx].d_gender, doctors[idx].d_specialization, doctors[idx].d_phone, doctors[idx].d_cnic, reg_time_str);
        }
        printf("-----------------------------------------------------------------------------------------------\n");

        printf("\nSelect the number of the doctor you want to update (1-%d): ", matchCount);
        int choice = inputInt("");
        while (choice < 1 || choice > matchCount)
        {
            printf("Invalid choice. Enter a number between 1 and %d: ", matchCount);
            choice = inputInt("");
        }
        select_index = matches[choice - 1];
    }

    updateDoctorHelper(select_index);
}

void updateDoctorHelper(int select_index)
{
    int updated = 0;

    while (1)
    {

        printf("\n--- Existing Info of doctor with ID %s ---\n", doctors[select_index].d_id);
        printf("1. Name           : %s\n", doctors[select_index].d_name);
        printf("2. Age            : %d\n", doctors[select_index].d_age);
        printf("3. Gender         : %s\n", doctors[select_index].d_gender);
        printf("4. Specialization : %s\n", doctors[select_index].d_specialization);
        printf("5. Contact Number : %s\n", doctors[select_index].d_phone);
        printf("6. CNIC          : %s\n", doctors[select_index].d_cnic);
        printf("7. << Go Back (Finish Updating)\n\n");

        int choice = inputInt("Which field do you want to change (1-7): ");
        while (choice < 1 || choice > 7)
        {
            printf("Invalid choice! Please enter a valid choice between 1 and 7.\n");
            choice = inputInt("Enter a choice: ");
        }

        if (choice == 7)
        {
            break;
        }

        const char *fieldName[] = {"Name", "Age", "Gender", "Specialization", "Contact Number", "CNIC"};
        printf("\nYou chose to update *%s*.\n", fieldName[choice - 1]);
        printf("1. Proceed to update\n");
        printf("2. Go back to menu\n");
        int sub_choice = inputInt("Enter your choice: ");

        if (sub_choice != 1)
        {
            continue;
        }

        switch (choice)
        {
        case 1:
            inputValidatedName(doctors[select_index].d_name, sizeof(doctors[select_index].d_name));
            break;
        case 2:
            doctors[select_index].d_age = inputValidatedAge();
            break;
        case 3:
            inputValidatedGender(doctors[select_index].d_gender, sizeof(doctors[select_index].d_gender));
            break;
        case 4:
            inputValidatedName(doctors[select_index].d_specialization, sizeof(doctors[select_index].d_specialization));
            break;
        case 5:
            inputValidatedContact(doctors[select_index].d_phone, sizeof(doctors[select_index].d_phone));
            break;
        case 6:
            inputValidatedCNIC(doctors[select_index].d_cnic, sizeof(doctors[select_index].d_cnic));
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
                saveDoctorsToFile();
                printf("\nDoctor record updated and saved successfully.\n");
            }
            else
            {
                printf("\nUpdate cancelled. No changes were saved.\n");
            }
        }
        else
        {
            printf("\nNo changes were made to the doctor record.\n");
        }
    }
}
