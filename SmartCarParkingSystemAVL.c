#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

typedef struct ParkingStatus_Tag                       //node is the structure for linked list to store status of parking space(0-empty,1-occupied)
{
    int parkingID;
    int status;
    int occ;
    int revenue;

    int height;
    struct ParkingStatus_Tag *left;
    struct ParkingStatus_Tag *right;
} Status;

typedef struct details_tag                     //details is the structure for linked list to store all info of entered cars
{
    char vn[20];                               //vn is unique vehicle number
    char name[50];                             //name is the name of user(driver)
    int memb;                                  //0-general,1-premium,2-golden
    float tpHours;                             //tpHours is total parking hours
    int psID;                                  //psID is the parking space ID

    struct tm arrival;
    struct tm departure;

    float totalAmountPaid;                     //total Amount Paid by user(user[i])
    float totalParkings;                       //total Parkings done by vehicle

    int height;
    struct details_tag *left;
    struct details_tag *right;
} details;

Status* createStatusNode(int parkingID);
int heightStatus(Status* N);
int maxHeight(int a, int b);
int getBalanceStatus(Status* N);
Status* rotateRightStatus(Status* y);
Status* rotateLeftStatus(Status* x);
Status* insertStatus(Status* node, Status* nptr);


int heightDetails(details* N);
int getBalanceDetails(details* N);
details* rotateRightDetails(details* y);
details* rotateLeftDetails(details* x);
details* insertDetails(details* node, details* newNode);

details* insertByAmount(details* node, details* user);
void sortUsersByAmount(details* root, details** sortedTree);
details* insertByParkings(details* node, details* user);
void sortUsersByParkings(details* root, details** sortedTree);
Status* insertByOccupancy(Status* root, Status* ps);
void sortParkingByOccupancy(Status* root, Status** sortedTree);
Status* insertByRevenue(Status* root, Status* ps);
void sortParkingByRevenue(Status* root, Status** sortedTree);

Status* SearchParkingSpace(Status* root,int id);
details* SearchUser(details* root,char *vehiNum, int *found);

void membershipPolicy(details* searchedUser);
void paymentPolicy(details* searchedUser,float pHoursceil,Status* rootStatus);
float parkingDuration(struct tm arrival ,struct tm depature);
int isEligible(int memb, int id);
void recursiveAlloHelper(Status* root, details* searchedUser, int* k, int* found);
int allopolicy(Status* rootStatus, details* searchedUser, int* k);
void deleteMemory(details* rootDetails, Status* rootStatus);
void deleteStatusTree(Status* root);
void deleteDetailsTree(details* root);

void displayParkingRecords(details* root);
void displaySortedUsersByAmount(details* sortedByAmount);
void displaySortedUsersByParkings(details* sortedByParkings);
void displaySortedDetailsRecursive(details* root);
void displayDetailsRecursive(details* root);
void printDetailsRow(details* user);

void displaySortedParkingSpaces(Status *sortedByOccupancy, Status *sortedByRevenue);
void printStatusRow(Status* root);

void loadDataFromFile(details** detailsRoot, Status** statusRoot);
void saveDetailsTree(FILE* file, details* root);
void saveStatusTree(FILE* file, Status* root);
void saveDataToFile(details* rootDetails, Status* rootStatus);

int main()
{
    Status* rootStatus = NULL;
    details* rootUser = NULL;

    FILE *f=fopen("user_tree_data.txt","r");
    if(!f)
    {
        for(int i=1;i<=50;i++)
        {
            Status* nptr = createStatusNode(i);
            rootStatus = insertStatus(rootStatus, nptr);
        }
        fclose(f);
    }

    // Load data from files
    loadDataFromFile(&rootUser, &rootStatus);

    //switch case
    int choice;
    int found = 0; 
    char vehiNo[20];
    int check2 = 0;
    int k;
    do 
    {
        printf("\nParking System Menu:\n");
        printf("1. User Interface (Vehicle Entry & Exit)\n");
        printf("2. Display Parking System Records\n");
        printf("3. Display Sorted Users by Amount Paid\n");
        printf("4. Display Sorted Users by Number of Parkings\n");
        printf("5. Display Sorted Parking Spaces (Revenue & Occupancy)\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) 
        {
            case 1:
                found = 0; 
                printf("Enter vehicle number: ");
                scanf("%s", vehiNo);
        
                details* rootUserDup ;                   //headUserDup is searched user if found
                rootUserDup=SearchUser(rootUser,vehiNo, &found);
                if (found==1)                                          //Registered
                {   
                    if (rootUserDup->psID != 0)                         // Departure 
                    {
                        printf("Enter user name for auhentication: ");
                        char name2[20];
                        scanf("%s", &name2);
                        if(!(strcmp(rootUserDup->name,name2)))
                        {
                            printf("Enter departure time and date (hours, minutes, seconds, day, month, year): ");
                            scanf("%d %d %d %d %d %d", &rootUserDup->departure.tm_hour, &rootUserDup->departure.tm_min,
                                &rootUserDup->departure.tm_sec, &rootUserDup->departure.tm_mday, &rootUserDup->departure.tm_mon,
                                &rootUserDup->departure.tm_year);
                            rootUserDup->departure.tm_mon -= 1;
                            rootUserDup->departure.tm_year -= 1900;
        
                            float pHours = parkingDuration(rootUserDup->arrival, rootUserDup->departure);
                            rootUserDup->tpHours += pHours;
                            membershipPolicy(rootUserDup);             //Giving membership if applicable
        
                            float pHoursceil = ceil(pHours);
                            paymentPolicy(rootUserDup, pHoursceil, rootStatus);//change revenue and amount paid in trees

                            int storeid = rootUserDup->psID;
                            Status* temp = SearchParkingSpace(rootStatus, storeid);
                            
                            temp->status = 0;
                            rootUserDup->psID = 0;

                            check2 = 0;
                            while(check2 != 1)
                            {
                                printf("\nIf Payment done(Enter 1) otherwise(Enter 0) :");
                                scanf("%d",&check2);
                            }
                        }
                        else
                        {
                            printf("User Authntication Failed\n");
                        }
                    } 
                    else                                           //Multiple entry
                    {
                        rootUserDup->totalParkings += 1;
                        printf("Enter arrival time and date (hours, minutes, seconds, day, month, year): ");
                        scanf("%d %d %d %d %d %d", &rootUserDup->arrival.tm_hour, &rootUserDup->arrival.tm_min,
                            &rootUserDup->arrival.tm_sec, &rootUserDup->arrival.tm_mday, &rootUserDup->arrival.tm_mon,
                            &rootUserDup->arrival.tm_year);
                        rootUserDup->arrival.tm_mon -= 1;
                        rootUserDup->arrival.tm_year -= 1900;
        
                        int flag = allopolicy(rootStatus, rootUserDup,&k);//update occupancy and total parkings
                        if (!flag) printf("Parking space is NOT empty\n");
        
                        printf("\nPark your car at ParkingSpace id %d",k);
                        check2 = 0;
                        while(check2 != 1)
                        {
                            printf("\nIf vehicle parked(Enter 1) otherwise(Enter 0) :");
                            scanf("%d",&check2);
                        }
                    }
                } 
                else                                                                    // New user registration OR first time entry
                {
                    details* nptr = (details*)malloc(sizeof(details));                    //new to to be joined to details
                    nptr->memb = 0;
                    int flag2 = allopolicy(rootStatus,nptr,&k);//update occupancy and total parkings
                    if(flag2 == 0)
                    {
                        printf("parking space in not available \n");   
                    }
                    else
                    {
                        if (nptr) {
                            strcpy(nptr->vn, vehiNo);

                            printf("Enter user name: ");
                            scanf("%s", nptr->name);
                            printf("Enter arrival time and date (hours, minutes, seconds, day, month, year): ");
                            scanf("%d %d %d %d %d %d", &nptr->arrival.tm_hour, &nptr->arrival.tm_min,
                                &nptr->arrival.tm_sec, &nptr->arrival.tm_mday, &nptr->arrival.tm_mon,
                                &nptr->arrival.tm_year);
                            nptr->arrival.tm_mon -= 1;
                            nptr->arrival.tm_year -= 1900;
                            nptr->departure.tm_hour = -1;
                            nptr->departure.tm_sec = -1;
                            nptr->memb = 0;
                            nptr->tpHours = 0;
                            nptr->totalAmountPaid = 0;
                            nptr->totalParkings = 1;
                            nptr->height = 1;
                            nptr->left = NULL;
                            nptr->right = NULL;
                            
                            rootUser = insertDetails(rootUser, nptr);

                            printf("\nPark your car at ParkingSpace id %d",k);
                            check2 = 0;
                            while(check2 != 1)
                            {
                                printf("\nIf vehicle parked(Enter 1) otherwise(Enter 0) :");
                                scanf("%d",&check2);
                            }
                        }
                        else
                        {
                            printf("FAILURE IN MEMORY ALLOCATION");
                        }             
                    }
                }
                found = 0;
                break;
            case 2:
                displayParkingRecords(rootUser);
                break;
            case 3:
                {
                    details* sortedByAmount = NULL;
                    sortUsersByAmount(rootUser, &sortedByAmount);
                    displaySortedUsersByAmount(sortedByAmount);
                    deleteDetailsTree(sortedByAmount);
                    break;
                }
            case 4:
                {
                    details* sortedByParkings;
                    sortedByParkings = NULL;
                    sortUsersByParkings(rootUser, &sortedByParkings);
                    displaySortedUsersByParkings(sortedByParkings);
                    deleteDetailsTree(sortedByParkings);
                    break;
                }
            case 5:
                {
                    Status* sortedByOccupancy = NULL;
                    sortParkingByOccupancy(rootStatus, &sortedByOccupancy);
                    Status* sortedByRevenue = NULL;
                    sortParkingByRevenue(rootStatus, &sortedByRevenue);
                    displaySortedParkingSpaces(sortedByOccupancy, sortedByRevenue);
                    deleteStatusTree(sortedByOccupancy);
                    deleteStatusTree(sortedByRevenue);
                break;
                }
            case 6:
                printf("Saving data and exiting...\n");
                saveDataToFile(rootUser, rootStatus);
                deleteMemory(rootUser, rootStatus);
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice, please try again.\n");
        }
    } while (choice != 6);

    return 0;
}

void loadDataFromFile(details** detailsRoot, Status** statusRoot) {
    FILE* file;

    // Load user AVL tree data
    file = fopen("user_tree_data.txt", "r");
    if (file) {
        while (!feof(file)) {
            details* nptr = (details*)malloc(sizeof(details));
            if (fscanf(file, "%s %s %d %f %d %f %f %d %d %d %d %d %d %d %d %d %d %d %d",
                nptr->vn, nptr->name, &nptr->memb, &nptr->tpHours, &nptr->psID,
                &nptr->totalAmountPaid, &nptr->totalParkings,
                &nptr->arrival.tm_hour, &nptr->arrival.tm_min, &nptr->arrival.tm_sec,
                &nptr->arrival.tm_mday, &nptr->arrival.tm_mon, &nptr->arrival.tm_year,
                &nptr->departure.tm_hour, &nptr->departure.tm_min, &nptr->departure.tm_sec,
                &nptr->departure.tm_mday, &nptr->departure.tm_mon, &nptr->departure.tm_year) == 19)
            {
                nptr->arrival.tm_mon -= 1;
                nptr->arrival.tm_year -= 1900;
                nptr->departure.tm_mon -= 1;
                nptr->departure.tm_year -= 1900;
                nptr->left = nptr->right = NULL;
                nptr->height = 1;

                int grew = 0;
                *detailsRoot = insertDetails(*detailsRoot, nptr); // Assuming this exists
            } else {
                free(nptr);
            }
        }
        fclose(file);
    }

    // Load parking status AVL tree data
    file = fopen("status_tree_data.txt", "r");
    if (file) {
        while (!feof(file)) {
            Status* nptr = (Status*)malloc(sizeof(Status));
            if (fscanf(file, "%d %d %d %d", &nptr->parkingID, &nptr->status, &nptr->occ, &nptr->revenue) == 4) {
                nptr->left = nptr->right = NULL;
                nptr->height = 1;

                *statusRoot = insertStatus(*statusRoot, nptr); // Assuming this exists
            } else {
                free(nptr);
            }
        }
        fclose(file);
    }
}
void saveDetailsTree(FILE* file, details* root) {
    if(root != NULL)
    {
        saveDetailsTree(file, root->left);

        fprintf(file, "%s %s %d %f %d %f %f %d %d %d %d %d %d %d %d %d %d %d %d\n",
            root->vn, root->name, root->memb, root->tpHours, root->psID,
            root->totalAmountPaid, root->totalParkings,
            root->arrival.tm_hour, root->arrival.tm_min, root->arrival.tm_sec,
            root->arrival.tm_mday, root->arrival.tm_mon + 1, root->arrival.tm_year + 1900,
            root->departure.tm_hour, root->departure.tm_min, root->departure.tm_sec,
            root->departure.tm_mday, root->departure.tm_mon + 1, root->departure.tm_year + 1900
        );

        saveDetailsTree(file, root->right);
    }
}
void saveStatusTree(FILE* file, Status* root) {
    if (root != NULL)
    {
        saveStatusTree(file, root->left);

        fprintf(file, "%d %d %d %d\n", root->parkingID, root->status, root->occ, root->revenue);

        saveStatusTree(file, root->right);
    }
}
// File handling functions
void saveDataToFile(details* rootDetails, Status* rootStatus) 
{
    FILE* file;

    // 🧾 Save User Details Tree
    file = fopen("user_tree_data.txt", "w");
    saveDetailsTree(file, rootDetails);
    fclose(file);

    // 🧾 Save Parking Status Tree
    file = fopen("status_tree_data.txt", "w");
    saveStatusTree(file, rootStatus);
    fclose(file);
}
void printStatusRow(Status* root) {
    if (root != NULL)
    {
        // In-order traversal: left → root → right
        printStatusRow(root->right);

        printf("%2d %3d Rs.%.2f\n", root->parkingID, root->occ, (float)root->revenue);

        printStatusRow(root->left);
    }
}
void displaySortedParkingSpaces(Status *sortedByOccupancy, Status *sortedByRevenue)
{
    // Display sorted by occupancy
    printf("=========== PARKING SPACES SORTED BY OCCUPANCY ===========\n\n");
    printf("--------------------------------------------------------------\n");
    printf("%-10s %-20s %-10s\n", "psID", "Number of Occupancies", "Revenue");
    printf("--------------------------------------------------------------\n");

    printStatusRow(sortedByOccupancy);

    printf("--------------------------------------------------------------\n");
    printf("\n===============================================================\n\n");

    // Display sorted by revenue
    printf("=========== PARKING SPACES SORTED BY REVENUE =============\n\n");
    printf("--------------------------------------------------------------\n");
    printf("%-10s %-20s %-10s\n", "psID", "Number of Occupancies", "Revenue");
    printf("--------------------------------------------------------------\n");

    printStatusRow(sortedByRevenue);

    printf("--------------------------------------------------------------\n");
    printf("\n===============================================================\n\n");
}
void displayDetailsRecursive(details* root) {
    if (root != NULL)
    {
        // In-order traversal (left → root → right)
        displayDetailsRecursive(root->left);
        printDetailsRow(root);
        displayDetailsRecursive(root->right);
    }
}
void displaySortedDetailsRecursive(details* root) {
    if (root != NULL)
    {
        // In-order traversal (left → root → right)
        displaySortedDetailsRecursive(root->right);
        printDetailsRow(root);
        displaySortedDetailsRecursive(root->left);
    }
}
void displayParkingRecords(details* root) {
    printf("\n");
    printf("\n============================================\n");
    printf("           PARKING SYSTEM RECORDS           \n");
    printf("============================================\n\n");

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("%-15s %-15s %-15s %-10s %-10s %-20s %-15s\n", 
        "Vehicle No.", "User Name", "Membership", "Hours", "psID", "Total Parkings", "Amount Paid");
    printf("--------------------------------------------------------------------------------------------------------\n");

    displayDetailsRecursive(root);

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("\n============================================\n\n");
}
void displaySortedUsersByAmount(details* sortedByAmount)
{
    printf("=========== VEHICLES SORTED BY AMOUNT PAID ===========\n\n");

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("%-15s %-15s %-15s %-10s %-10s %-20s %-15s\n", 
        "Vehicle No.", "User Name", "Membership", "Hours", "psID", "Total Parkings", "Amount Paid");
    printf("--------------------------------------------------------------------------------------------------------\n");

    displaySortedDetailsRecursive(sortedByAmount);

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("\n=========================================================\n\n");
}
void displaySortedUsersByParkings(details* sortedByParkings)
{
    printf("=========== VEHICLES SORTED BY NUMBER OF PARKINGS ===========\n\n");

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("%-15s %-15s %-15s %-10s %-10s %-20s %-15s\n", 
        "Vehicle No.", "User Name", "Membership", "Hours", "psID", "Total Parkings", "Amount Paid");
    printf("--------------------------------------------------------------------------------------------------------\n");

    displaySortedDetailsRecursive(sortedByParkings);

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("\n===============================================================\n\n");
}
void printDetailsRow(details* user) {
    printf("%-15s %-15s ", user->vn, user->name);

    if (user->memb == 0)
        printf("%-15s ", "No Membership");
    else if (user->memb == 1)
        printf("%-15s ", "Premium");
    else
        printf("%-15s ", "Golden");

    printf("%-10.2f %-10d %-20.2f %-15.2f\n", 
           user->tpHours, user->psID, 
           user->totalParkings, user->totalAmountPaid);
}


Status* createStatusNode(int parkingID) 
{
    Status* newNode = (Status*)malloc(sizeof(Status));
    newNode->parkingID = parkingID;
    newNode->status = 0;
    newNode->occ = 0;
    newNode->revenue = 0;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

int heightStatus(Status* N)
{
    int ret_val;
    if (N == NULL)
    {
        ret_val = 0;
    }
    else
    {
        ret_val = N->height;
    }
}
int maxHeight(int a, int b)
{
    int ret_val;
    if(a > b)
    {
        ret_val = a;
    }
    else
    {
        ret_val = b;
    }
    return b;
}
int getBalanceStatus(Status* N)
{
    int ret_val;
    if (N == NULL)
    {
        ret_val =  0;
    }
    else
    {
        ret_val =  heightStatus(N->left) - heightStatus(N->right);
    } 
    return ret_val;
}

Status* rotateRightStatus(Status* y) {
    Status* x = y->left;
    Status* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = maxHeight(heightStatus(y->left), heightStatus(y->right)) + 1;
    x->height = maxHeight(heightStatus(x->left), heightStatus(x->right)) + 1;

    return x;
}

Status* rotateLeftStatus(Status* x) {
    Status* y = x->right;
    Status* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = maxHeight(heightStatus(x->left), heightStatus(x->right)) + 1;
    y->height = maxHeight(heightStatus(y->left), heightStatus(y->right)) + 1;

    return y;
}


Status* insertStatus(Status* node, Status* nptr) {
    Status* ret_val;

    if (node == NULL) {
        ret_val = nptr;
    } 
    else {
        if (nptr->parkingID < node->parkingID) {
            node->left = insertStatus(node->left, nptr);
        } 
        else if (nptr->parkingID > node->parkingID) {
            node->right = insertStatus(node->right, nptr);
        } 
        else {
            // Duplicate key: no insertion, return the node itself
            ret_val = node;
        }

        // Only balance if not duplicate
        if (nptr->parkingID != node->parkingID) {
            // Update height
            node->height = 1 + maxHeight(heightStatus(node->left), heightStatus(node->right));

            int balance = getBalanceStatus(node);

            // Handle rotations
            if (balance > 1 && nptr->parkingID < node->left->parkingID) {
                ret_val = rotateRightStatus(node); // LL
            } 
            else if (balance < -1 && nptr->parkingID > node->right->parkingID) {
                ret_val = rotateLeftStatus(node); // RR
            } 
            else if (balance > 1 && nptr->parkingID > node->left->parkingID) {
                node->left = rotateLeftStatus(node->left); // LR
                ret_val = rotateRightStatus(node);
            } 
            else if (balance < -1 && nptr->parkingID < node->right->parkingID) {
                node->right = rotateRightStatus(node->right); // RL
                ret_val = rotateLeftStatus(node);
            } 
            else {
                ret_val = node; // No rotation needed
            }
        }
    }

    return ret_val;
}

int heightDetails(details* N) {
    int ret_val;
    if (N == NULL)
    {
        ret_val = 0;
    }
    else
    {
        ret_val = N->height;
    }
    return ret_val;
}
int getBalanceDetails(details* N) {
    int ret_val;
    if (N == NULL)
    {
        ret_val =  0;
    }
    else
    {
        ret_val =  heightDetails(N->left) - heightDetails(N->right);
    } 
    return ret_val;
}

details* rotateRightDetails(details* y) {
    details* x = y->left;
    details* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = maxHeight(heightDetails(y->left), heightDetails(y->right)) + 1;
    x->height = maxHeight(heightDetails(x->left), heightDetails(x->right)) + 1;

    return x;
}

details* rotateLeftDetails(details* x) {
    details* y = x->right;
    details* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = maxHeight(heightDetails(x->left), heightDetails(x->right)) + 1;
    y->height = maxHeight(heightDetails(y->left), heightDetails(y->right)) + 1;

    return y;
}
details* insertDetails(details* node, details* newNode) {
    details* ret_val;

    if (node == NULL) {
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        ret_val = newNode;
    } else {
        int cmp = strcmp(newNode->vn, node->vn);
        if (cmp < 0) {
            node->left = insertDetails(node->left, newNode);
        } else if (cmp > 0) {
            node->right = insertDetails(node->right, newNode);
        } else {
            // Duplicate VN not allowed
            ret_val = node;
        }

        if (cmp != 0) {
            // Update height
            node->height = 1 + maxHeight(heightDetails(node->left), heightDetails(node->right));

            // Get balance factor
            int balance = getBalanceDetails(node);

            // LL
            if (balance > 1 && strcmp(newNode->vn, node->left->vn) < 0)
                ret_val = rotateRightDetails(node);
            // RR
            else if (balance < -1 && strcmp(newNode->vn, node->right->vn) > 0)
                ret_val = rotateLeftDetails(node);
            // LR
            else if (balance > 1 && strcmp(newNode->vn, node->left->vn) > 0) {
                node->left = rotateLeftDetails(node->left);
                ret_val = rotateRightDetails(node);
            }
            // RL
            else if (balance < -1 && strcmp(newNode->vn, node->right->vn) < 0) {
                node->right = rotateRightDetails(node->right);
                ret_val = rotateLeftDetails(node);
            } else {
                ret_val = node;
            }
        }
    }

    return ret_val;
}
details* insertByAmount(details* node, details* user) {
    details* ret_val;

    if (node == NULL) {
        details* newNode = (details*)malloc(sizeof(details));
        strcpy(newNode->vn, user->vn);
        strcpy(newNode->name, user->name);
        newNode->psID = user->psID;
        newNode->tpHours = user->tpHours;
        newNode->totalParkings = user->totalParkings;
        newNode->totalAmountPaid = user->totalAmountPaid;
        newNode->memb = user->memb;

        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        ret_val = newNode;
    } else {
        if (user->totalAmountPaid < node->totalAmountPaid)
            node->left = insertByAmount(node->left, user);
        else
            node->right = insertByAmount(node->right, user); // Equal goes to right

        // Update height
        node->height = 1 + maxHeight(heightDetails(node->left), heightDetails(node->right));

        // Get balance factor
        int balance = getBalanceDetails(node);

        // LL
        if (balance > 1 && user->totalAmountPaid < node->left->totalAmountPaid)
            ret_val = rotateRightDetails(node);
        // RR
        else if (balance < -1 && user->totalAmountPaid >= node->right->totalAmountPaid)
            ret_val = rotateLeftDetails(node);
        // LR
        else if (balance > 1 && user->totalAmountPaid >= node->left->totalAmountPaid) {
            node->left = rotateLeftDetails(node->left);
            ret_val = rotateRightDetails(node);
        }
        // RL
        else if (balance < -1 && user->totalAmountPaid < node->right->totalAmountPaid) {
            node->right = rotateRightDetails(node->right);
            ret_val = rotateLeftDetails(node);
        } else {
            ret_val = node;
        }
    }

    return ret_val;
}
void sortUsersByAmount(details* root, details** sortedTree)
{
    if(root != NULL)
    {
        sortUsersByAmount(root->left, sortedTree);
        *sortedTree = insertByAmount(*sortedTree, root);
        sortUsersByAmount(root->right, sortedTree);
    }
}

details* insertByParkings(details* node, details* user) {
    details* ret_val;

    if (node == NULL) {
        details* newNode = (details*)malloc(sizeof(details));
        strcpy(newNode->vn, user->vn);
        strcpy(newNode->name, user->name);
        newNode->psID = user->psID;
        newNode->tpHours = user->tpHours;
        newNode->totalParkings = user->totalParkings;
        newNode->totalAmountPaid = user->totalAmountPaid;
        newNode->memb = user->memb;
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        ret_val = newNode;
    } else {
        if (user->totalParkings < node->totalParkings)
            node->left = insertByParkings(node->left, user);
        else
            node->right = insertByParkings(node->right, user); // equal goes right

        // Update height
        node->height = 1 + maxHeight(heightDetails(node->left), heightDetails(node->right));

        // Get balance factor
        int balance = getBalanceDetails(node);

        // LL
        if (balance > 1 && user->totalParkings < node->left->totalParkings)
            ret_val = rotateRightDetails(node);
        // RR
        else if (balance < -1 && user->totalParkings >= node->right->totalParkings)
            ret_val = rotateLeftDetails(node);
        // LR
        else if (balance > 1 && user->totalParkings >= node->left->totalParkings) {
            node->left = rotateLeftDetails(node->left);
            ret_val = rotateRightDetails(node);
        }
        // RL
        else if (balance < -1 && user->totalParkings < node->right->totalParkings) {
            node->right = rotateRightDetails(node->right);
            ret_val = rotateLeftDetails(node);
        } else {
            ret_val = node;
        }
    }

    return ret_val;
}

void sortUsersByParkings(details* root, details** sortedTree)
{
    if(root != NULL)
    {
        sortUsersByParkings(root->left, sortedTree);
        *sortedTree = insertByParkings(*sortedTree, root);
        sortUsersByParkings(root->right, sortedTree);
    }
}

Status* insertByOccupancy(Status* node, Status* ps) {
    Status* ret_val;

    if (node == NULL) {
        Status* newNode = (Status*)malloc(sizeof(Status));
        newNode->parkingID = ps->parkingID;
        newNode->occ = ps->occ;
        newNode->revenue = ps->revenue;
        newNode->status = ps->status;
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        ret_val = newNode;
    } else {
        if (ps->occ < node->occ)
            node->left = insertByOccupancy(node->left, ps);
        else
            node->right = insertByOccupancy(node->right, ps); // equal goes right

        // Update height
        node->height = 1 + maxHeight(heightStatus(node->left), heightStatus(node->right));

        // Get balance factor
        int balance = getBalanceStatus(node);

        // LL
        if (balance > 1 && ps->occ < node->left->occ)
            ret_val = rotateRightStatus(node);
        // RR
        else if (balance < -1 && ps->occ >= node->right->occ)
            ret_val = rotateLeftStatus(node);
        // LR
        else if (balance > 1 && ps->occ >= node->left->occ) {
            node->left = rotateLeftStatus(node->left);
            ret_val = rotateRightStatus(node);
        }
        // RL
        else if (balance < -1 && ps->occ < node->right->occ) {
            node->right = rotateRightStatus(node->right);
            ret_val = rotateLeftStatus(node);
        } else {
            ret_val = node;
        }
    }

    return ret_val;
}
void sortParkingByOccupancy(Status* root, Status** sortedTree)
{
    if(root != NULL)
    {
        sortParkingByOccupancy(root->left, sortedTree);
        *sortedTree = insertByOccupancy(*sortedTree, root);
        sortParkingByOccupancy(root->right, sortedTree);
    }
}
Status* insertByRevenue(Status* node, Status* ps) {
    Status* ret_val;

    if (node == NULL) {
        Status* newNode = (Status*)malloc(sizeof(Status));
        newNode->parkingID = ps->parkingID;
        newNode->status = ps->status;
        newNode->occ = ps->occ;
        newNode->revenue = ps->revenue;
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        ret_val = newNode;
    } else {
        if (ps->revenue < node->revenue)
            node->left = insertByRevenue(node->left, ps);
        else
            node->right = insertByRevenue(node->right, ps); // equal goes right

        // Update height
        node->height = 1 + maxHeight(heightStatus(node->left), heightStatus(node->right));

        // Get balance factor
        int balance = getBalanceStatus(node);

        // LL
        if (balance > 1 && ps->revenue < node->left->revenue)
            ret_val = rotateRightStatus(node);
        // RR
        else if (balance < -1 && ps->revenue >= node->right->revenue)
            ret_val = rotateLeftStatus(node);
        // LR
        else if (balance > 1 && ps->revenue >= node->left->revenue) {
            node->left = rotateLeftStatus(node->left);
            ret_val = rotateRightStatus(node);
        }
        // RL
        else if (balance < -1 && ps->revenue < node->right->revenue) {
            node->right = rotateRightStatus(node->right);
            ret_val = rotateLeftStatus(node);
        }
        else {
            ret_val = node;
        }
    }

    return ret_val;
}
void sortParkingByRevenue(Status* root, Status** sortedTree)
{
    if(root != NULL)
    {
        sortParkingByRevenue(root->left, sortedTree);
        *sortedTree = insertByRevenue(*sortedTree, root);
        sortParkingByRevenue(root->right, sortedTree);
    }
}

void deleteDetailsTree(details* root)
{
    if (root != NULL)
    {
        deleteDetailsTree(root->left);
        deleteDetailsTree(root->right);
        free(root);
    }
}
void deleteStatusTree(Status* root) {
    if (root != NULL)
    {
        deleteStatusTree(root->left);
        deleteStatusTree(root->right);
        free(root);
    }
}
details* SearchUser(details* root,char *vehiNum, int *found)
{
    details* ret_val = NULL;
    if(root != NULL)
    {
        int cmp = strcmp(vehiNum, root->vn);
        if(cmp < 0)
        {
            ret_val = SearchUser(root->left, vehiNum, found);
        }
        else if(cmp > 0)
        {
            ret_val = SearchUser(root->right, vehiNum, found);
        }
        else
        {
            ret_val = root;
            *found = 1;
        }
    }
    return ret_val;
}
Status* SearchParkingSpace(Status* root,int id)
{
    Status* ret_val = NULL;
    if(root != NULL)
    {
        if(root->parkingID > id)
        {
            ret_val = SearchParkingSpace(root->left, id);
        }
        else if(root->parkingID < id)
        {
            ret_val = SearchParkingSpace(root->right, id);
        }
        else
        {
            ret_val = root;
        }
    }
    return ret_val;
}

int isEligible(int membership, int parkingID) {
    int ret_val = 0;
    if ((parkingID >= 1 && parkingID <= 10) && membership == 2) {
        ret_val = 1;
    }
    if ((parkingID > 10 && parkingID <= 20) && (membership == 1 || membership == 2)) {
        ret_val = 1;
    }
    if ((parkingID > 20 && parkingID <= 50)) {
        ret_val = 1;
    }
    return ret_val;
}
void recursiveAlloHelper(Status* root, details* searchedUser, int* k, int* found) {
    if (root != NULL && *found == 0) {

        // Recurse into left subtree
        if (root->left != NULL)
            recursiveAlloHelper(root->left, searchedUser, k, found);

        // Process current node if still not found
        if (*found == 0 && root->status == 0 && isEligible(searchedUser->memb, root->parkingID)) {
            root->status = 1;
            root->occ += 1;
            searchedUser->psID = root->parkingID;
            *k = root->parkingID;
            *found = 1;
        }

        // Recurse into right subtree if still not found
        if (*found == 0 && root->right != NULL)
            recursiveAlloHelper(root->right, searchedUser, k, found);
    }
}
int allopolicy(Status* rootStatus, details* searchedUser, int* k) {
    int found = 0;
    recursiveAlloHelper(rootStatus, searchedUser, k, &found);
    return found;
}

float parkingDuration(struct tm arrival ,struct tm depature)
{
    time_t time1 = mktime(&arrival);
    time_t time2 = mktime(&depature);
    float seconds_diff = difftime(time2,time1);
    float hour_diff = (seconds_diff)/3600;
    return hour_diff;
}
void membershipPolicy(details* searchedUser)
{
    if(searchedUser->tpHours>=100)
    {
        if(searchedUser->tpHours>=200)
        {
            searchedUser->memb=2;
        }
        else
        {
            searchedUser->memb=1;
        }
    }
}
void paymentPolicy(details* searchedUser,float pHoursceil,Status* rootStatus)
{
    float amountPaid;           //amount paid at each time
    if(pHoursceil<=3)
    {
        amountPaid=100.00*pHoursceil;
    }
    else
    {
        amountPaid=(300.00)+(50.00*(pHoursceil-3));
    }    
    if(searchedUser->memb==1 || searchedUser->memb==2)
    {
        amountPaid=(0.9)*amountPaid;
    }
    searchedUser->totalAmountPaid+=amountPaid;

    Status* temp= SearchParkingSpace(rootStatus, searchedUser->psID);
    
    temp->revenue+=amountPaid;
    printf("Your payble amount is %.2f Rupees",amountPaid);
}

void deleteMemory(details* rootDetails, Status* rootStatus)
{
    deleteDetailsTree(rootDetails);
    deleteStatusTree(rootStatus);
}