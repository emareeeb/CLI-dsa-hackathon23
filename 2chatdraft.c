#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define MAX_AIRCRAFT 100

// Define a structure to represent an aircraft
typedef struct {
    int aircraft_id;
    double latitude;
    double longitude;
    double altitude;
    double speed;
    double destination_latitude;
    double destination_longitude;
    double urgency;  // Added urgency field
} Aircraft;

// Define a structure to represent an airspace sector
typedef struct {
    double min_latitude;
    double max_latitude;
    double min_longitude;
    double max_longitude;
} AirspaceSector;

// Define a structure to represent a priority queue node
typedef struct {
    Aircraft aircraft;
    double priority; // Priority based on urgency
} PriorityQueueNode;

// Define a structure for the priority queue
typedef struct {
    PriorityQueueNode heap[MAX_AIRCRAFT];
    int size;
} PriorityQueue;

// Function prototype for calculateDistance
double calculateDistance(double lat1, double lon1, double lat2, double lon2);

// Function prototypes for the priority queue
void initializePriorityQueue(PriorityQueue* pq);
void insertIntoPriorityQueue(PriorityQueue* pq, Aircraft aircraft);
Aircraft removeFromPriorityQueue(PriorityQueue* pq);

// Function prototype for conflict resolution
void resolveConflict(Aircraft* aircraft1, Aircraft* aircraft2);

// Define a structure to represent the air traffic control system
typedef struct {
    PriorityQueue priority_queue;        // Priority queue to manage aircraft
    AirspaceSector sectors[3];           // Example: Three sectors for demonstration purposes
} AirTrafficControlSystem;

// Function to initialize the air traffic control system
void initializeATCSystem(AirTrafficControlSystem* atcSystem) {
    initializePriorityQueue(&atcSystem->priority_queue);

    // Example sectorization
    atcSystem->sectors[0] = (AirspaceSector){30.0, 40.0, -120.0, -110.0};
    atcSystem->sectors[1] = (AirspaceSector){30.0, 40.0, -110.0, -100.0};
    atcSystem->sectors[2] = (AirspaceSector){40.0, 50.0, -120.0, -100.0};
}

// Function to add a new aircraft to the system
void addAircraft(AirTrafficControlSystem* atcSystem, int aircraft_id, double latitude, double longitude, double altitude, double speed, double dest_latitude, double dest_longitude) {
    Aircraft newAircraft = {aircraft_id, latitude, longitude, altitude, speed, dest_latitude, dest_longitude, 0.0};
    insertIntoPriorityQueue(&atcSystem->priority_queue, newAircraft);
    printf("Aircraft %d added to the system.\n", aircraft_id);
}

// Function to check for conflicts and resolve them
void checkAndResolveConflicts(AirTrafficControlSystem* atcSystem) {
    while (atcSystem->priority_queue.size > 1) {
        Aircraft aircraft1 = removeFromPriorityQueue(&atcSystem->priority_queue);
        Aircraft aircraft2 = atcSystem->priority_queue.heap[0].aircraft;

        int sector_i = -1, sector_j = -1;

        for (int k = 0; k < 3; ++k) {
            if (aircraft1.latitude >= atcSystem->sectors[k].min_latitude &&
                aircraft1.latitude <= atcSystem->sectors[k].max_latitude &&
                aircraft1.longitude >= atcSystem->sectors[k].min_longitude &&
                aircraft1.longitude <= atcSystem->sectors[k].max_longitude) {
                sector_i = k;
            }

            if (aircraft2.latitude >= atcSystem->sectors[k].min_latitude &&
                aircraft2.latitude <= atcSystem->sectors[k].max_latitude &&
                aircraft2.longitude >= atcSystem->sectors[k].min_longitude &&
                aircraft2.longitude <= atcSystem->sectors[k].max_longitude) {
                sector_j = k;
            }
        }

        if (sector_i != -1 && sector_i == sector_j) {
            double distance = calculateDistance(aircraft1.latitude, aircraft1.longitude,
                                                aircraft2.latitude, aircraft2.longitude);

            if (distance < 1000) { // Example threshold for demonstration purposes
                printf("Conflict detected between Aircraft %d and Aircraft %d in Sector %d. Taking evasive action...\n",
                       aircraft1.aircraft_id, aircraft2.aircraft_id, sector_i);

                // Resolve conflict
                resolveConflict(&aircraft1, &aircraft2);
                insertIntoPriorityQueue(&atcSystem->priority_queue, aircraft1);
                insertIntoPriorityQueue(&atcSystem->priority_queue, aircraft2);

                printf("Conflict resolved. Aircraft %d now at altitude %lf, Aircraft %d now at altitude %lf\n",
                       aircraft1.aircraft_id, aircraft1.altitude, aircraft2.aircraft_id, aircraft2.altitude);
            }
        }
    }
}

// Function to resolve a conflict (e.g., change altitude)
void resolveConflict(Aircraft* aircraft1, Aircraft* aircraft2) {
    // Example: Increase altitude by 1000 for both aircraft
    aircraft1->altitude += 1000;
    aircraft2->altitude += 1000;
}

// Function to calculate the distance between two points on the globe using Haversine formula
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // Earth radius in kilometers
    double dlat = (lat2 - lat1) * (M_PI / 180.0);
    double dlon = (lon2 - lon1) * (M_PI / 180.0);
    double a = sin(dlat / 2.0) * sin(dlat / 2.0) + cos(lat1 * (M_PI / 180.0)) * cos(lat2 * (M_PI / 180.0)) * sin(dlon / 2.0) * sin(dlon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    return R * c;
}

// Function to display information about all aircraft in the system
void displayAllAircraft(AirTrafficControlSystem* atcSystem) {
    printf("Current Aircraft in the System:\n");
    for (int i = 0; i < atcSystem->priority_queue.size; ++i) {
        Aircraft aircraft = atcSystem->priority_queue.heap[i].aircraft;
        printf("Aircraft %d - Latitude: %lf, Longitude: %lf, Altitude: %lf, Speed: %lf, Destination: (%lf, %lf), Urgency: %lf\n",
               aircraft.aircraft_id, aircraft.latitude, aircraft.longitude, aircraft.altitude,
               aircraft.speed, aircraft.destination_latitude, aircraft.destination_longitude, aircraft.urgency);
    }
}

// Function to initialize the priority queue
void initializePriorityQueue(PriorityQueue* pq) {
    pq->size = 0;
}

// Function to insert an aircraft into the priority queue
void insertIntoPriorityQueue(PriorityQueue* pq, Aircraft aircraft) {
    if (pq->size < MAX_AIRCRAFT) {
        PriorityQueueNode newNode;
        newNode.aircraft = aircraft;
        // Priority is calculated based on urgency
        newNode.priority = aircraft.urgency;
        pq->heap[pq->size++] = newNode;
        int current = pq->size - 1;

        // Heapify up
        while (current > 0) {
            int parent = (current - 1) / 2;
            if (pq->heap[current].priority > pq->heap[parent].priority) {
                // Swap nodes if the current node has higher priority
                PriorityQueueNode temp = pq->heap[current];
                pq->heap[current] = pq->heap[parent];
                pq->heap[parent] = temp;
                current = parent;
            } else {
                break;
            }
        }
    } else {
        printf("Error: Maximum capacity reached.\n");
    }
}

// Function to remove the aircraft with the highest priority from the priority queue
Aircraft removeFromPriorityQueue(PriorityQueue* pq) {
    Aircraft removedAircraft = pq->heap[0].aircraft;
    pq->heap[0] = pq->heap[--pq->size];
    int current = 0;

    // Heapify down
    while (2 * current + 1 < pq->size) {
        int leftChild = 2 * current + 1;
        int rightChild = 2 * current + 2;
        int largerChild;

        if (rightChild < pq->size && pq->heap[rightChild].priority > pq->heap[leftChild].priority) {
            largerChild = rightChild;
        } else {
            largerChild = leftChild;
        }

        if (pq->heap[largerChild].priority > pq->heap[current].priority) {
            // Swap nodes if the larger child has higher priority
            PriorityQueueNode temp = pq->heap[current];
            pq->heap[current] = pq->heap[largerChild];
            pq->heap[largerChild] = temp;
            current = largerChild;
        } else {
            break;
        }
    }

    return removedAircraft;
}

int main() {
    AirTrafficControlSystem atcSystem;
    initializeATCSystem(&atcSystem);

    // Adding some sample aircraft with destinations
    int aircraft_id;
    double latitude, longitude, altitude, speed, dest_latitude, dest_longitude;

    // Input for Aircraft 1
    printf("Enter details for Aircraft 1:\n");
    printf("Aircraft ID: ");
    scanf("%d", &aircraft_id);
    printf("Latitude: ");
    scanf("%lf", &latitude);
    printf("Longitude: ");
    scanf("%lf", &longitude);
    printf("Altitude: ");
    scanf("%lf", &altitude);
    printf("Speed: ");
    scanf("%lf", &speed);
    printf("Destination Latitude: ");
    scanf("%lf", &dest_latitude);
    printf("Destination Longitude: ");
    scanf("%lf", &dest_longitude);

    addAircraft(&atcSystem, aircraft_id, latitude, longitude, altitude, speed, dest_latitude, dest_longitude);

    // Input for Aircraft 2
    printf("Enter details for Aircraft 2:\n");
    printf("Aircraft ID: ");
    scanf("%d", &aircraft_id);
    printf("Latitude: ");
    scanf("%lf", &latitude);
    printf("Longitude: ");
    scanf("%lf", &longitude);
    printf("Altitude: ");
    scanf("%lf", &altitude);
    printf("Speed: ");
    scanf("%lf", &speed);
    printf("Destination Latitude: ");
    scanf("%lf", &dest_latitude);
    printf("Destination Longitude: ");
    scanf("%lf", &dest_longitude);

    addAircraft(&atcSystem, aircraft_id, latitude, longitude, altitude, speed, dest_latitude, dest_longitude);

    // Displaying all aircraft in the system
    displayAllAircraft(&atcSystem);

    // Check for conflicts and resolve them
    checkAndResolveConflicts(&atcSystem);

    // Displaying all aircraft in the system after conflict resolution
    displayAllAircraft(&atcSystem);

    return 0;
}