#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME_LEN 50
#define MAX_GENDER_LEN 10
#define MAX_STATUS_LEN 20
#define MAX_SEATS_LIMIT 100
#define BUFFER_SIZE 128

typedef struct WaitingNode {
    int pnr;
    char name[MAX_NAME_LEN];
    int age;
    char gender[MAX_GENDER_LEN];
    int trainNumber;
    float fare;
    struct WaitingNode* next;
} WaitingNode;

typedef struct WaitingQueue {
    WaitingNode* front;
    WaitingNode* rear;
    int count;
} WaitingQueue;

typedef struct Train {
    int trainNumber;
    char trainName[MAX_NAME_LEN];
    char source[MAX_NAME_LEN];
    char destination[MAX_NAME_LEN];
    int totalSeats;
    int availableSeats;
    float fare;
    int seatMap[MAX_SEATS_LIMIT + 1];
    WaitingQueue waitingQueue;
    struct Train* next;
} Train;

typedef struct Passenger {
    int pnr;
    char name[MAX_NAME_LEN];
    int age;
    char gender[MAX_GENDER_LEN];
    int trainNumber;
    int seatNumber;
    float fare;
    char status[MAX_STATUS_LEN];
    struct Passenger* next;
} Passenger;

Train* trainHead = NULL;
Passenger* passengerHead = NULL;
int nextPNR = 1001;

void readSafeString(char* buffer, int maxLen) {
    if (fgets(buffer, maxLen, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        buffer[0] = '\0';
    }
}

int readSafeInt(int* out) {
    char buffer[BUFFER_SIZE];
    readSafeString(buffer, BUFFER_SIZE);

    char* ptr = buffer;
    while (isspace((unsigned char)*ptr)) ptr++;
    if (*ptr == '\0') return 0;

    char* endptr;
    long val = strtol(ptr, &endptr, 10);
    while (isspace((unsigned char)*endptr)) endptr++;
    if (*endptr != '\0') return 0;

    *out = (int)val;
    return 1;
}

void initQueue(WaitingQueue* q) {
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

int isQueueEmpty(WaitingQueue* q) {
    return (q->front == NULL);
}

void enqueueWaitingPassenger(WaitingQueue* q, int pnr, const char* name, int age, const char* gender, int trainNumber, float fare) {
    WaitingNode* newNode = (WaitingNode*)malloc(sizeof(WaitingNode));
    if (!newNode) {
        fprintf(stderr, "Error: Memory allocation failed for WaitingNode.\n");
        exit(EXIT_FAILURE);
    }
    newNode->pnr = pnr;
    strncpy(newNode->name, name, MAX_NAME_LEN - 1);
    newNode->name[MAX_NAME_LEN - 1] = '\0';
    newNode->age = age;
    strncpy(newNode->gender, gender, MAX_GENDER_LEN - 1);
    newNode->gender[MAX_GENDER_LEN - 1] = '\0';
    newNode->trainNumber = trainNumber;
    newNode->fare = fare;
    newNode->next = NULL;

    if (isQueueEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->count++;
}

WaitingNode* dequeueWaitingPassenger(WaitingQueue* q) {
    if (isQueueEmpty(q)) {
        return NULL;
    }

    WaitingNode* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    q->count--;
    return temp;
}

Train* createTrainNode(int trainNumber, const char* name, const char* src, const char* dest, int totalSeats, float fare) {
    Train* newTrain = (Train*)malloc(sizeof(Train));
    if (!newTrain) {
        fprintf(stderr, "Error: Memory allocation failed for Train.\n");
        exit(EXIT_FAILURE);
    }
    newTrain->trainNumber = trainNumber;
    strncpy(newTrain->trainName, name, MAX_NAME_LEN - 1);
    newTrain->trainName[MAX_NAME_LEN - 1] = '\0';
    strncpy(newTrain->source, src, MAX_NAME_LEN - 1);
    newTrain->source[MAX_NAME_LEN - 1] = '\0';
    strncpy(newTrain->destination, dest, MAX_NAME_LEN - 1);
    newTrain->destination[MAX_NAME_LEN - 1] = '\0';

    newTrain->totalSeats = totalSeats;
    newTrain->availableSeats = totalSeats;
    newTrain->fare = fare;

    for (int i = 0; i <= MAX_SEATS_LIMIT; i++) {
        newTrain->seatMap[i] = 0;
    }

    initQueue(&(newTrain->waitingQueue));
    newTrain->next = NULL;
    return newTrain;
}

void addTrain(int trainNumber, const char* name, const char* src, const char* dest, int totalSeats, float fare) {
    Train* newNode = createTrainNode(trainNumber, name, src, dest, totalSeats, fare);
    if (trainHead == NULL) {
        trainHead = newNode;
    } else {
        Train* curr = trainHead;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = newNode;
    }
}

Train* findTrain(int trainNumber) {
    Train* curr = trainHead;
    while (curr != NULL) {
        if (curr->trainNumber == trainNumber) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

int allocateSeat(Train* t) {
    for (int seat = 1; seat <= t->totalSeats; seat++) {
        if (t->seatMap[seat] == 0) {
            t->seatMap[seat] = 1;
            return seat;
        }
    }
    return -1;
}

void freeSeat(Train* t, int seatNumber) {
    if (seatNumber >= 1 && seatNumber <= t->totalSeats) {
        t->seatMap[seatNumber] = 0;
    }
}

void initializeTrains(void) {
    addTrain(101, "Chennai Express",     "Chennai", "Bangalore",  50, 450.0f);
    addTrain(102, "Coastal Express",     "Chennai", "Coimbatore", 40, 350.0f);
    addTrain(103, "Intercity Express",   "Chennai", "Madurai",    60, 400.0f);
    addTrain(104, "Southern Express",    "Chennai", "Trichy",     50, 300.0f);
    addTrain(105, "City Express",        "Chennai", "Salem",       3, 250.0f);
}

int generatePNR(void) {
    return nextPNR++;
}

Passenger* createPassengerNode(int pnr, const char* name, int age, const char* gender, int trainNumber, int seatNumber, float fare, const char* status) {
    Passenger* p = (Passenger*)malloc(sizeof(Passenger));
    if (!p) {
        fprintf(stderr, "Error: Memory allocation failed for Passenger.\n");
        exit(EXIT_FAILURE);
    }
    p->pnr = pnr;
    strncpy(p->name, name, MAX_NAME_LEN - 1);
    p->name[MAX_NAME_LEN - 1] = '\0';
    p->age = age;
    strncpy(p->gender, gender, MAX_GENDER_LEN - 1);
    p->gender[MAX_GENDER_LEN - 1] = '\0';
    p->trainNumber = trainNumber;
    p->seatNumber = seatNumber;
    p->fare = fare;
    strncpy(p->status, status, MAX_STATUS_LEN - 1);
    p->status[MAX_STATUS_LEN - 1] = '\0';
    p->next = NULL;
    return p;
}

void addPassengerRecord(Passenger* newP) {
    if (passengerHead == NULL) {
        passengerHead = newP;
    } else {
        Passenger* curr = passengerHead;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = newP;
    }
}

Passenger* findPassenger(int pnr) {
    Passenger* curr = passengerHead;
    while (curr != NULL) {
        if (curr->pnr == pnr) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void displayTrains(void) {
    if (trainHead == NULL) {
        printf("\nNo train records available.\n");
        return;
    }

    printf("\n----------------------------------------------------------------------------------------\n");
    printf("%-10s %-22s %-12s %-14s %-8s %-10s %-8s\n",
           "TRAIN NO.", "TRAIN NAME", "SOURCE", "DESTINATION", "TOTAL", "AVAILABLE", "FARE(Rs)");
    printf("----------------------------------------------------------------------------------------\n");

    Train* curr = trainHead;
    while (curr != NULL) {
        printf("%-10d %-22s %-12s %-14s %-8d %-10d %-8.2f\n",
               curr->trainNumber,
               curr->trainName,
               curr->source,
               curr->destination,
               curr->totalSeats,
               curr->availableSeats,
               curr->fare);
        curr = curr->next;
    }
    printf("----------------------------------------------------------------------------------------\n");
}

void searchTrain(void) {
    int trainNum;
    printf("\nEnter Train Number to search: ");
    if (!readSafeInt(&trainNum)) {
        printf("ERROR: Invalid input! Please enter a numerical train number.\n");
        return;
    }

    Train* t = findTrain(trainNum);
    if (t != NULL) {
        printf("\n========================================================\n");
        printf("                      TRAIN FOUND                       \n");
        printf("========================================================\n");
        printf("Train Number    : %d\n", t->trainNumber);
        printf("Train Name      : %s\n", t->trainName);
        printf("Source          : %s\n", t->source);
        printf("Destination     : %s\n", t->destination);
        printf("Total Seats     : %d\n", t->totalSeats);
        printf("Available Seats : %d\n", t->availableSeats);
        printf("Booked Seats    : %d\n", (t->totalSeats - t->availableSeats));
        printf("Waiting List    : %d\n", t->waitingQueue.count);
        printf("Fare per Ticket : Rs. %.2f\n", t->fare);
        printf("========================================================\n");
    } else {
        printf("\nERROR: Train %d not found in the railway schedule.\n", trainNum);
    }
}

void sortTrains(void) {
    if (trainHead == NULL || trainHead->next == NULL) {
        printf("\nNot enough trains to sort.\n");
        return;
    }

    int choice;
    printf("\n--- SORT TRAINS ---\n");
    printf("1. Sort by Train Number\n");
    printf("2. Sort by Available Seats (Descending)\n");
    printf("3. Sort by Fare (Ascending)\n");
    printf("Enter your choice: ");

    if (!readSafeInt(&choice) || choice < 1 || choice > 3) {
        printf("ERROR: Invalid sorting option.\n");
        return;
    }

    int count = 0;
    Train* curr = trainHead;
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }

    Train** arr = (Train**)malloc(count * sizeof(Train*));
    curr = trainHead;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int swapNeeded = 0;

            if (choice == 1) {
                if (arr[j]->trainNumber > arr[j + 1]->trainNumber) swapNeeded = 1;
            } else if (choice == 2) {
                if (arr[j]->availableSeats < arr[j + 1]->availableSeats) swapNeeded = 1;
            } else if (choice == 3) {
                if (arr[j]->fare > arr[j + 1]->fare) swapNeeded = 1;
            }

            if (swapNeeded) {
                Train* temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    trainHead = arr[0];
    for (int i = 0; i < count - 1; i++) {
        arr[i]->next = arr[i + 1];
    }
    arr[count - 1]->next = NULL;
    free(arr);

    printf("\nTrains sorted successfully using Bubble Sort!\n");
    displayTrains();
}

void bookTicket(void) {
    int trainNum, age;
    char name[MAX_NAME_LEN];
    char gender[MAX_GENDER_LEN];

    printf("\nEnter Train Number to book: ");
    if (!readSafeInt(&trainNum)) {
        printf("ERROR: Invalid train number.\n");
        return;
    }

    Train* t = findTrain(trainNum);
    if (t == NULL) {
        printf("ERROR: Train with number %d does not exist.\n", trainNum);
        return;
    }

    printf("Enter Passenger Name: ");
    readSafeString(name, MAX_NAME_LEN);
    if (strlen(name) == 0) {
        printf("ERROR: Passenger name cannot be blank.\n");
        return;
    }

    printf("Enter Passenger Age: ");
    if (!readSafeInt(&age) || age <= 0 || age > 120) {
        printf("ERROR: Please enter a valid age (1-120).\n");
        return;
    }

    printf("Enter Gender (Male/Female/Other): ");
    readSafeString(gender, MAX_GENDER_LEN);
    if (strlen(gender) == 0) {
        strncpy(gender, "Unspecified", MAX_GENDER_LEN - 1);
        gender[MAX_GENDER_LEN - 1] = '\0';
    }

    int pnr = generatePNR();

    if (t->availableSeats > 0) {
        int seat = allocateSeat(t);
        t->availableSeats--;

        Passenger* newP = createPassengerNode(pnr, name, age, gender, trainNum, seat, t->fare, "CONFIRMED");
        addPassengerRecord(newP);

        printf("\n========================================================\n");
        printf("                    BOOKING CONFIRMED                   \n");
        printf("========================================================\n");
        printf("PNR Number  : %d\n", pnr);
        printf("Passenger   : %s\n", name);
        printf("Age / Gender: %d / %s\n", age, gender);
        printf("Train No    : %d (%s)\n", t->trainNumber, t->trainName);
        printf("Route       : %s -> %s\n", t->source, t->destination);
        printf("Seat No     : %d\n", seat);
        printf("Fare        : Rs. %.2f\n", t->fare);
        printf("Status      : CONFIRMED\n");
        printf("========================================================\n");
    } else {
        enqueueWaitingPassenger(&(t->waitingQueue), pnr, name, age, gender, trainNum, t->fare);

        Passenger* newP = createPassengerNode(pnr, name, age, gender, trainNum, 0, t->fare, "WAITING");
        addPassengerRecord(newP);

        printf("\n========================================================\n");
        printf("                    WAITING LIST                        \n");
        printf("========================================================\n");
        printf("No confirmed seats available on Train %d.\n", trainNum);
        printf("Passenger added to Waiting Queue (FIFO).\n\n");
        printf("PNR Number  : %d\n", pnr);
        printf("Passenger   : %s\n", name);
        printf("Train No    : %d (%s)\n", t->trainNumber, t->trainName);
        printf("Status      : WAITING\n");
        printf("Waiting Pos : %d\n", t->waitingQueue.count);
        printf("========================================================\n");
    }
}

void cancelTicket(void) {
    int pnr;
    printf("\nEnter PNR Number to cancel: ");
    if (!readSafeInt(&pnr)) {
        printf("ERROR: Invalid PNR format.\n");
        return;
    }

    Passenger* p = findPassenger(pnr);
    if (p == NULL) {
        printf("\nERROR: No reservation found with PNR %d.\n", pnr);
        return;
    }

    if (strcmp(p->status, "CANCELLED") == 0) {
        printf("\nERROR: Ticket with PNR %d is already CANCELLED.\n", pnr);
        return;
    }

    Train* t = findTrain(p->trainNumber);
    if (t == NULL) {
        printf("\nERROR: Associated train record not found.\n");
        return;
    }

    if (strcmp(p->status, "WAITING") == 0) {
        strncpy(p->status, "CANCELLED", MAX_STATUS_LEN - 1);
        printf("\nWaiting list ticket PNR %d has been CANCELLED.\n", pnr);
        return;
    }

    int freedSeat = p->seatNumber;
    strncpy(p->status, "CANCELLED", MAX_STATUS_LEN - 1);
    p->seatNumber = -1;
    freeSeat(t, freedSeat);
    t->availableSeats++;

    printf("\n========================================================\n");
    printf("              TICKET CANCELLED SUCCESSFULLY             \n");
    printf("========================================================\n");
    printf("PNR Number  : %d\n", pnr);
    printf("Passenger   : %s\n", p->name);
    printf("Seat Freed  : %d\n", freedSeat);
    printf("Status      : CANCELLED\n");
    printf("========================================================\n");

    if (!isQueueEmpty(&(t->waitingQueue))) {
        WaitingNode* promoted = dequeueWaitingPassenger(&(t->waitingQueue));
        if (promoted != NULL) {
            int assignedSeat = allocateSeat(t);
            t->availableSeats--;

            Passenger* wp = findPassenger(promoted->pnr);
            if (wp != NULL) {
                wp->seatNumber = assignedSeat;
                strncpy(wp->status, "CONFIRMED", MAX_STATUS_LEN - 1);
            }

            printf("\n>>> WAITING LIST PROMOTION (FIFO QUEUE) <<<\n");
            printf("A seat opened up! The next passenger in the waiting queue has been promoted:\n");
            printf("Promoted PNR   : %d\n", promoted->pnr);
            printf("Passenger Name : %s\n", promoted->name);
            printf("Train Number   : %d\n", promoted->trainNumber);
            printf("Assigned Seat  : %d\n", assignedSeat);
            printf("New Status     : CONFIRMED\n");
            printf("Remaining Wait : %d passengers\n", t->waitingQueue.count);
            printf("========================================================\n");

            free(promoted);
        }
    }
}

void searchReservation(void) {
    int pnr;
    printf("\nEnter PNR Number to search: ");
    if (!readSafeInt(&pnr)) {
        printf("ERROR: Invalid PNR.\n");
        return;
    }

    Passenger* p = findPassenger(pnr);
    if (p == NULL) {
        printf("\nNo reservation record found for PNR %d.\n", pnr);
        return;
    }

    Train* t = findTrain(p->trainNumber);

    printf("\n========================================================\n");
    printf("                  RESERVATION DETAILS                   \n");
    printf("========================================================\n");
    printf("PNR Number    : %d\n", p->pnr);
    printf("Passenger     : %s\n", p->name);
    printf("Age / Gender  : %d / %s\n", p->age, p->gender);
    printf("Train Number  : %d %s\n", p->trainNumber, (t ? t->trainName : ""));
    if (t != NULL) {
        printf("Route         : %s -> %s\n", t->source, t->destination);
    }
    if (strcmp(p->status, "CONFIRMED") == 0) {
        printf("Seat Number   : %d\n", p->seatNumber);
    } else if (strcmp(p->status, "WAITING") == 0) {
        printf("Seat Number   : Waiting in Queue\n");
    } else {
        printf("Seat Number   : Cancelled\n");
    }
    printf("Fare          : Rs. %.2f\n", p->fare);
    printf("Status        : %s\n", p->status);
    printf("========================================================\n");
}

void displayConfirmedReservations(void) {
    if (passengerHead == NULL) {
        printf("\nNo passenger records found.\n");
        return;
    }

    int count = 0;
    printf("\n------------------------------------------------------------------------------------\n");
    printf("%-8s %-20s %-10s %-8s %-12s %-12s\n",
           "PNR", "PASSENGER NAME", "TRAIN NO.", "SEAT NO.", "FARE (Rs)", "STATUS");
    printf("------------------------------------------------------------------------------------\n");

    Passenger* curr = passengerHead;
    while (curr != NULL) {
        if (strcmp(curr->status, "CONFIRMED") == 0) {
            printf("%-8d %-20s %-10d %-8d %-12.2f %-12s\n",
                   curr->pnr,
                   curr->name,
                   curr->trainNumber,
                   curr->seatNumber,
                   curr->fare,
                   curr->status);
            count++;
        }
        curr = curr->next;
    }
    printf("------------------------------------------------------------------------------------\n");
    printf("Total Confirmed Passengers: %d\n", count);
}

void displayWaitingList(void) {
    int totalWaiting = 0;
    Train* t = trainHead;

    printf("\n========================================================================\n");
    printf("                   TRAIN WAITING LIST (FIFO QUEUES)                     \n");
    printf("========================================================================\n");

    while (t != NULL) {
        printf("\nTrain %d - %s (%s -> %s) [Waiting Count: %d]\n",
               t->trainNumber, t->trainName, t->source, t->destination, t->waitingQueue.count);

        if (isQueueEmpty(&(t->waitingQueue))) {
            printf("  (No passengers on waiting list)\n");
        } else {
            printf("  --------------------------------------------------------------------\n");
            printf("  %-10s %-8s %-20s %-6s %-10s\n", "POSITION", "PNR", "PASSENGER", "AGE", "GENDER");
            printf("  --------------------------------------------------------------------\n");

            int pos = 1;
            WaitingNode* w = t->waitingQueue.front;
            while (w != NULL) {
                printf("  %-10d %-8d %-20s %-6d %-10s\n",
                       pos++, w->pnr, w->name, w->age, w->gender);
                totalWaiting++;
                w = w->next;
            }
            printf("  --------------------------------------------------------------------\n");
        }
        t = t->next;
    }
    printf("\nTotal Waiting Passengers Across All Trains: %d\n", totalWaiting);
    printf("========================================================================\n");
}

void displayAvailability(void) {
    int trainNum;
    printf("\nEnter Train Number to check availability: ");
    if (!readSafeInt(&trainNum)) {
        printf("ERROR: Invalid input.\n");
        return;
    }

    Train* t = findTrain(trainNum);
    if (t == NULL) {
        printf("ERROR: Train %d not found.\n", trainNum);
        return;
    }

    int bookedSeats = t->totalSeats - t->availableSeats;

    printf("\n========================================================\n");
    printf("               TRAIN SEAT AVAILABILITY                  \n");
    printf("========================================================\n");
    printf("Train Number    : %d\n", t->trainNumber);
    printf("Train Name      : %s\n", t->trainName);
    printf("Route           : %s -> %s\n", t->source, t->destination);
    printf("Total Seats     : %d\n", t->totalSeats);
    printf("Booked Seats    : %d\n", bookedSeats);
    printf("Available Seats : %d\n", t->availableSeats);
    printf("Waiting List    : %d\n", t->waitingQueue.count);
    printf("Status          : %s\n", (t->availableSeats > 0 ? "AVAILABLE" : "WAITING LIST ONLY"));
    printf("========================================================\n");
}

void freeMemory(void) {
    Passenger* pCurr = passengerHead;
    while (pCurr != NULL) {
        Passenger* pTemp = pCurr;
        pCurr = pCurr->next;
        free(pTemp);
    }
    passengerHead = NULL;

    Train* tCurr = trainHead;
    while (tCurr != NULL) {
        WaitingNode* wCurr = tCurr->waitingQueue.front;
        while (wCurr != NULL) {
            WaitingNode* wTemp = wCurr;
            wCurr = wCurr->next;
            free(wTemp);
        }

        Train* tTemp = tCurr;
        tCurr = tCurr->next;
        free(tTemp);
    }
    trainHead = NULL;
}

int main(void) {
    initializeTrains();
    int choice = 0;

    printf("\n");
    printf("========================================================\n");
    printf("          TRAIN RESERVATION MANAGEMENT SYSTEM           \n");
    printf("    A DSA-Based Ticket Booking & Management System      \n");
    printf("========================================================\n");

    while (1) {
        printf("\n========================================================\n");
        printf("          TRAIN RESERVATION MANAGEMENT SYSTEM           \n");
        printf("========================================================\n");
        printf("1. Display All Trains\n");
        printf("2. Search Train\n");
        printf("3. Sort Trains\n");
        printf("4. Book Ticket\n");
        printf("5. Cancel Ticket\n");
        printf("6. Search Reservation\n");
        printf("7. Display Confirmed Reservations\n");
        printf("8. Display Waiting List\n");
        printf("9. Display Train Availability\n");
        printf("10. Exit\n");
        printf("========================================================\n");
        printf("Enter your choice: ");

        if (!readSafeInt(&choice)) {
            printf("\nERROR: Invalid input! Please enter a number between 1 and 10.\n");
            continue;
        }

        switch (choice) {
            case 1:
                displayTrains();
                break;
            case 2:
                searchTrain();
                break;
            case 3:
                sortTrains();
                break;
            case 4:
                bookTicket();
                break;
            case 5:
                cancelTicket();
                break;
            case 6:
                searchReservation();
                break;
            case 7:
                displayConfirmedReservations();
                break;
            case 8:
                displayWaitingList();
                break;
            case 9:
                displayAvailability();
                break;
            case 10:
                printf("\nExiting Train Reservation Management System...\n");
                printf("Freeing all allocated dynamic memory (Linked Lists & Queues)...\n");
                freeMemory();
                printf("Thank you for using the Train Reservation System! Goodbye.\n\n");
                return 0;
            default:
                printf("\nERROR: Invalid choice! Please select an option from 1 to 10.\n");
                break;
        }
    }

    return 0;
}
