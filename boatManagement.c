//
//  main.c
//  boatManagementAssignment1.c
//
//  Created by Eltonia Leonard
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enumeration definition to list the possible boat locations
typedef enum
{
    slip,
    land,
    trailor,
    storage,
    no_place
}
PlaceType;

// Union definition to store additional information about each boat's location
typedef union
{
    int slip_number;               // Slip number (to store the slip number 1-85)
    char bay_letter;               // The bay letter that would be from A-Z
    char trailor_license_tag[20];  // Trailor license tag for the boats
    int storage_space_number;      // The storage space number for the boats
}
extra_info;

// Structure definition to hold the complete information about a boat
typedef struct
{
    char boat_name[128];
    int length;
    PlaceType location;
    extra_info details;
    float money_owed_to_marina;
}
boat;

// Constants to manage boats and monthly rates.
#define MAX_BOATS 120       // Maximum number of boats allowed in the marina
#define SLIP_RATE 12.5      // Monthly rate for boats in slips
#define LAND_RATE 14.0      // Monthly rate for boats on land
#define TRAILOR_RATE 25.0   // Monthly rate for boats on trailors
#define STORAGE_RATE 11.2   // Monthly rate for boats in general storage

// Converts a string to the corresponding location type (handling case-insensitive here)
PlaceType StringToPlaceType(char *location_string)
{
    // Compare the input string with each possible location type
    // If it matches, return the corresponding location type
    if (!strcasecmp(location_string, "slip"))
    {
        return slip;
    }
    if (!strcasecmp(location_string, "land"))
    {
        return land;
    }
    if (!strcasecmp(location_string, "trailor"))
    {
        return trailor;
    }
    if (!strcasecmp(location_string, "storage"))
    {
        return storage;
    }
    // If no match is found, return "no_place"
    return no_place;
}

// Converts a location type to its string representation
char *PlaceToString(PlaceType location)
{
    // Switch statement to determine the string representation of each location type
    // Return the corresponding string.
    switch (location)
    {
        case slip:
            return("slip");
        case land:
            return("land");
        case trailor:
            return("trailor");
        case storage:
            return("storage");
        case no_place:
            return("no_place");
        default:
            printf("Uh-oh. Invalid location type encountered\n");
            exit(EXIT_FAILURE);
            break;
    }
}

// Loads boats from a CSV file into the marina array
void load_boats(boat* marina[], const char* filename)
{
    // Try to open the file in read mode.
    FILE* file = fopen(filename, "r");

    // If the file cannot be opened, print an error message and return
    if (!file)
    {
        printf("Uh-oh. Error: Failed to open the file for reading.\n");
        return;
    }

    // Initialize a buffer to read lines from the file
    char line[256];
    // Start at the first slot in the marina array
    int index = 0;

    // Read each line from the file until the end is reached or the marina is full
    while (fgets(line, sizeof(line), file) && index < MAX_BOATS)
    {
        // Allocate memory for a new boat
        boat* new_boat = (boat*)malloc(sizeof(boat));

        // Check if memory allocation was successful
        if (!new_boat)
        {
            printf("Error: Memory allocation has failed.\n");
            break;
        }

        // Get the boat name, length, and location from the line
        char location_str[20];
        int num_parsed = sscanf(line, "%127[^,],%d,%19[^,],", new_boat->boat_name, &new_boat->length, location_str);
        if (num_parsed != 3)
        {
            printf("Error: Invalid line format in CSV.\n");
            free(new_boat);
            continue;
        }

        // Convert the location string to a PlaceType enum
        new_boat->location = StringToPlaceType(location_str);

        // Parse additional details based on location type
        int extra_info_parsed = 0;
        switch (new_boat->location)
        {
            case slip:
                extra_info_parsed = sscanf(line, "%*[^,],%*d,%*[^,],%d,%f", &new_boat->details.slip_number, &new_boat->money_owed_to_marina);
                break;
            case land:
                extra_info_parsed = sscanf(line, "%*[^,],%*d,%*[^,],%c,%f", &new_boat->details.bay_letter, &new_boat->money_owed_to_marina);
                break;
            case trailor:
                extra_info_parsed = sscanf(line, "%*[^,],%*d,%*[^,],%19[^,],%f", new_boat->details.trailor_license_tag, &new_boat->money_owed_to_marina);
                break;
            case storage:
                extra_info_parsed = sscanf(line, "%*[^,],%*d,%*[^,],%d,%f", &new_boat->details.storage_space_number, &new_boat->money_owed_to_marina);
                break;
            default:
                printf("Error: Invalid location type.\n");
                free(new_boat);
                continue;
        }

        // Check if extra info parsing was successful (must be 2 fields parsed)
        if (extra_info_parsed != 2)
        {
            printf("Error: Invalid extra details format in CSV.\n");
            free(new_boat);
            continue;
        }

        // Add the new boat to the marina array
        marina[index++] = new_boat;
    }

    // Close the file after loading is complete
    fclose(file);
}


// Saves boat data from the marina array to a CSV file
void save_boats(boat* marina[], const char* filename)
{
    // Try to open the file in write mode
    FILE* file = fopen(filename, "w");

    // If the file cannot be opened, display an error message and return
    if (!file)
    {
        printf("Error: Failed to open file for writing.\n");
        return;
    }

    // Go through each boat in the marina and write its details to the file
    for (int i = 0; i < MAX_BOATS; i++)
    {
        if (marina[i])
        {
            // Write the boats information
            fprintf(file, "%s,%d,%s,", marina[i]->boat_name, marina[i]->length, PlaceToString(marina[i]->location));

        
            switch (marina[i]->location)
            {
                case slip:
                    fprintf(file, "%d,", marina[i]->details.slip_number);
                    break;
                case land:
                    fprintf(file, "%c,", marina[i]->details.bay_letter);
                    break;
                case trailor:
                    fprintf(file, "%s,", marina[i]->details.trailor_license_tag);
                    break;
                case storage:
                    fprintf(file, "%d,", marina[i]->details.storage_space_number);
                    break;
                default:
                    break;
            }

            // Write the outstanding balance for the boat
            fprintf(file, "%.2f\n", marina[i]->money_owed_to_marina);
        }
    }

    // Once saving is complete, close the file
    fclose(file);
}

// Insert a new boat into the marina array based on input from user
void add_boat(boat* marina[], char* data)
{
    // Find the first empty slot in the marina array
    int index = 0;
    while (index < MAX_BOATS && marina[index]) index++;

    // If the marina is full, display an error message and return
    if (index == MAX_BOATS)
    {
        printf("Error: Marina has reached maximum capacity.\n");
        return;
    }

    // Allocate memory for a new boat
    boat* new_boat = (boat*)malloc(sizeof(boat));

    // Check if memory allocation was successful
    if (!new_boat)
    {
        printf("Error: Memory allocation has failed.\n");
        return;
    }

    // Get the boat name, length, and location from the input data
    char location_str[20];
    int num_parsed = sscanf(data, "%127[^,],%d,%19[^,],", new_boat->boat_name, &new_boat->length, location_str);
    if (num_parsed != 3)
    {
        printf("Error: Invalid input format.\n");
        free(new_boat);
        return;
    }

    // Convert the location string to a PlaceType enum
    new_boat->location = StringToPlaceType(location_str);

    // Based on the location type, handle additional details
    int extra_info_parsed = 0;
    switch (new_boat->location)
    {
        case slip:
            extra_info_parsed = sscanf(data, "%*[^,],%*d,%*[^,],%d,%f", &new_boat->details.slip_number, &new_boat->money_owed_to_marina);
            break;
        case land:
            extra_info_parsed = sscanf(data, "%*[^,],%*d,%*[^,],%c,%f", &new_boat->details.bay_letter, &new_boat->money_owed_to_marina);
            break;
        case trailor:
            extra_info_parsed = sscanf(data, "%*[^,],%*d,%*[^,],%19[^,],%f", new_boat->details.trailor_license_tag, &new_boat->money_owed_to_marina);
            break;
        case storage:
            extra_info_parsed = sscanf(data, "%*[^,],%*d,%*[^,],%d,%f", &new_boat->details.storage_space_number, &new_boat->money_owed_to_marina);
            break;
        default:
            printf("Error: Invalid location type.\n");
            free(new_boat);
            return;
    }

    // Ensure that extra info is parsed correctly
    if (extra_info_parsed != 2)
    {
        printf("Error: Invalid extra details format.\n");
        free(new_boat);
        return;
    }

    // Add the new boat to the marina array
    marina[index] = new_boat;
    printf("Boat added successfully!\n");
}

// Function to remove a boat from the marina based on the boat's name
void remove_boat(boat* marina[], char* boat_name)
{
    // Iterate through the marina to find the boat
    for (int i = 0; i < MAX_BOATS; i++)
    {
        // Check whether or not the boat exists and if it matches the given name
        if (marina[i] && strcasecmp(marina[i]->boat_name, boat_name) == 0)
        {
            // Free the memory that is occupied by the boat
            free(marina[i]);
            marina[i] = NULL;
            printf("The boat has beeen removed successfully!\n");
            return;
        }
    }

    // If no matching boat is found, display an error message to the user
    printf("Uh-oh. No boats with that name. \n");
}

// Function to update the monthly charges for all boats in the marina
void update_monthly_charges(boat* marina[])
{
    for (int i = 0; i < MAX_BOATS; i++)
    {
        if (marina[i])
        {
            float rate = 0;
            switch (marina[i]->location)
            {
                case slip:
                    rate = SLIP_RATE;
                    break;
                case land:
                    rate = LAND_RATE;
                    break;
                case trailor:
                    rate = TRAILOR_RATE;
                    break;
                case storage:
                    rate = STORAGE_RATE;
                    break;
                default:
                    printf("Error: Unknown location type.\n");
                    continue;
            }

            // Update the balance with the monthly charge
            marina[i]->money_owed_to_marina += rate * marina[i]->length;
        }
    }
    printf("Monthly charges updated.\n");
}

// Processes a payment for a specific boat based on its name
void process_payment(boat* marina[], char* boat_name)
{
    int boat_found = 0;
    float amount = 0.0;

    // Search for the boat by name
    for (int i = 0; i < MAX_BOATS; i++)
    {
        if (marina[i] && strcasecmp(marina[i]->boat_name, boat_name) == 0)
        {
            boat_found = 1;
            
            // Prompt the user for payment amount
            printf("Please enter the amount to be paid: ");
            scanf("%f", &amount);
            getchar();

            // Check if the payment exceeds the due amount
            if (amount > marina[i]->money_owed_to_marina)
            {
                printf("That is more than the amount owed, $%.2f\n", marina[i]->money_owed_to_marina);
            }
            else
            {
                // Deduct the payment from the balance
                marina[i]->money_owed_to_marina -= amount;
                printf("Payment successful. New balance is $%.2f\n", marina[i]->money_owed_to_marina);
            }
            return;
        }
    }

    // If no matching boat is found, display an error message
    if (!boat_found)
    {
        printf("Uh-oh. No boats with that name.\n");
    }
}

//  Function to perform a comparison of two boats by name for sorting
int compare_boats(const void* a, const void* b)
{
    // Cast void pointers to boat pointers for comparison
    boat* boat_a = *(boat**)a;
    boat* boat_b = *(boat**)b;
    
    // Compare boat names
    return strcasecmp(boat_a->boat_name, boat_b->boat_name);
}


// Function to display the current inventory of boats in the marina
void show_inventory(boat* marina[])
{
    // Create a temporary array to hold only non-NULL boats
    boat* temp_marina[MAX_BOATS];
    int count = 0;

    // Copy non-NULL pointers from the marina array to the temporary array
    for (int i = 0; i < MAX_BOATS; i++)
    {
        if (marina[i])
        {
            temp_marina[count++] = marina[i];
        }
    }

    // Sort the temporary array
    qsort(temp_marina, count, sizeof(boat*), compare_boats);

    // Display the boats
    for (int i = 0; i < count; i++)
    {
        printf("%-20s %2d'    %-8s ", temp_marina[i]->boat_name, temp_marina[i]->length, PlaceToString(temp_marina[i]->location));

        switch (temp_marina[i]->location) {
            case slip:
                printf("# %-3d  ", temp_marina[i]->details.slip_number);
                break;
            case land:
                printf("     %c   ", temp_marina[i]->details.bay_letter);
                break;
            case trailor:
                printf("%-7s ", temp_marina[i]->details.trailor_license_tag);
                break;
            case storage:
                printf("# %-3d  ", temp_marina[i]->details.storage_space_number);
                break;
            default:
                printf("Unknown ");
                break;
        }

        printf("  Owes $%6.2f\n", temp_marina[i]->money_owed_to_marina);
    }
}

// Main function to run the Boat Management System
int main(int argc, char* argv[])
{
    // Check if the correct number of command line arguments is provided
    if (argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        // Exit with an error code
        return 1;
    }
    
    // Use the filename specified by the user
    const char* filename = argv[1];

    // Display a welcome message to user when the program starts
    printf("Welcome to the Boat Management System!\n");
    printf("--------------------------------------\n");

    // Initialize the marina array to hold boat pointers
    boat* marina[MAX_BOATS] = { NULL };

    // Load boats from the specified file
    load_boats(marina, filename);

    // Variable that stores the user's menu choice
    char choice;

    
    // Loop to display the menu and handle input from user
    do
    {
        // Display the menu options to user
        printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it: ");
        
        // Get input from user
        choice = getchar();
        getchar();

        // Handle each menu option
        if (choice == 'i' || choice == 'I')
        {
            // Show the current boat inventory
            show_inventory(marina);
        }
        else if (choice == 'a' || choice == 'A')
        {
            // Add a new boat to the marina
            char data[256];
            printf("Enter boat data in CSV format: ");
            fgets(data, 256, stdin);
            add_boat(marina, data);
        }
        else if (choice == 'r' || choice == 'R')
        {
            // Remove a boat from the marina using its name
            char boat_name[128];
            printf("Enter boat name: ");
            fgets(boat_name, 128, stdin);
            boat_name[strcspn(boat_name, "\n")] = 0;
            remove_boat(marina, boat_name);
        }
        else if (choice == 'p' || choice == 'P')
        {
            // Process a payment for a specific boat
            char boat_name[128];
            printf("Please enter boat name: ");
            fgets(boat_name, 128, stdin);
            boat_name[strcspn(boat_name, "\n")] = 0;
            process_payment(marina, boat_name);
        }
        else if (choice == 'm' || choice == 'M')
        {
            // Update monthly charges for all boats in the marina
            update_monthly_charges(marina);
            printf("Monthly charges updated.\n");
        }
        else if (choice != 'x' && choice != 'X')
        {
            printf("Invalid option: %c\n", choice);
        }
    }
    // Continue until user chooses to exit
    while (choice != 'x' && choice != 'X');

    // Save the updated marina to the file before exiting
    save_boats(marina, filename);

    // Display an exit message to user
    printf("Exiting the Boat Management System.\n");

    // Free all allocated memory for boats before exiting
    for (int i = 0; i < MAX_BOATS; i++)
    {
        if (marina[i]) free(marina[i]);
    }

    return 0;
}
