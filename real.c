#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#define MAX_CODE_LENGTH 4

typedef struct {
    int id;
    char currentCode[MAX_CODE_LENGTH];
    double altitude;
    double speed;
    char destinationCode[MAX_CODE_LENGTH];
    int remainingFuel;
    char weatherCondition[20]; // Added weather condition
} Flight;

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct {
    int numNodes;
    char** airportCodes;
    Node** adjList;
} Graph;

// Function to create a graph
Graph create(int numNodes, char** codes) {
    Graph graph;
    graph.numNodes = numNodes;
    graph.airportCodes = codes;
    graph.adjList = (Node*)malloc(numNodes * sizeof(Node));
    for (int i = 0; i < numNodes; ++i) {
        graph.adjList[i] = NULL;
    }

    return graph;
}

// Function to add an edge between two nodes in the graph
void add(Graph* graph, int source, int destination) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = destination;
    newNode->next = graph->adjList[source];
    graph->adjList[source] = newNode;

    newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = source;
    newNode->next = graph->adjList[destination];
    graph->adjList[destination] = newNode;
}

// Function to find the closest node to a given node in the graph
int closest(Graph* graph, int source) {
    int minWeight = INT_MAX;
    int nearestNode = -1;
    Node* current = graph->adjList[source];

    while (current != NULL) {
        if (minWeight > 1) {
            minWeight = 1;
            nearestNode = current->vertex;
        }
        current = current->next;
    }
    return nearestNode;
}

// Function to handle collision between flights
void collision(Flight* flights, int numFlights) {
    for (int i = 0; i < numFlights - 1; ++i) {
        for (int j = i + 1; j < numFlights; ++j) {
            if (strcmp(flights[i].currentCode, flights[j].destinationCode) == 0 &&
                strcmp(flights[j].currentCode, flights[i].destinationCode) == 0) {
                flights[i].altitude += 1000.0;
                flights[j].altitude += 2000.0;

                printf("\nCollision detected between Flight %d and Flight %d. Altitudes adjusted.\n",
                    flights[i].id, flights[j].id);
            }
        }
    }
}

// Function to update the flight's current node based on the graph
void update(Flight* flights, int numFlights, Graph* graph) {
    for (int i = 0; i < numFlights; ++i) {
        if (strcmp(flights[i].currentCode, flights[i].destinationCode) != 0) {
            Node* current = graph->adjList[flights[i].currentCode[0] - 'A'];
            while (current != NULL) {
                flights[i].currentCode[0] = graph->airportCodes[current->vertex][0];
                break;
            }
        }
    }
}

// Function to find the closest node for flights with fuel shortages
void findclosest(Flight* flights, int numFlights, Graph* graph) {
    int expectedFuelConsumption = 15;

    for (int i = 0; i < numFlights; ++i) {
        if (flights[i].remainingFuel < expectedFuelConsumption) {
            printf("\nFlight Details for Flight %d:\n", flights[i].id);
            printf("| %-15s | %-15s | %-15s | %-15s | %-15s | %-15s | %-20s |\n", "Departure", "Altitude", "Speed", "Arrival", "Remaining Fuel", "Status", "Weather Condition");
            printf("| %-15s | %-15.2f | %-15.2f | %-15s | %-15d | %-15s | %-20s |\n",
                flights[i].currentCode, flights[i].altitude, flights[i].speed,
                flights[i].destinationCode, flights[i].remainingFuel, "Fuel Shortage", flights[i].weatherCondition);

            int nearestNode = closest(graph, flights[i].currentCode[0] - 'A');

            if (nearestNode != -1)
                printf("The nearest node for Flight %d to land is %c.\n", flights[i].id, graph->airportCodes[nearestNode][0]);
            else
                printf("No suitable node found for landing for Flight %d.\n", flights[i].id);

            // Suggest delay based on weather condition
            if (strcmp(flights[i].weatherCondition, "bad") == 0) {
                printf("Suggestion: Due to bad weather conditions, consider delaying Flight %d.\n", flights[i].id);
            }
        }
        else {
            printf("\nFlight Details for Flight %d:\n", flights[i].id);
            printf("| %-15s | %-15s | %-15s | %-15s | %-15s | %-15s | %-20s |\n", "Departure", "Altitude", "Speed", "Arrival", "Remaining Fuel", "Status", "Weather Condition");
            printf("| %-15s | %-15.2f | %-15.2f | %-15s |  %-15d | %-15s | %-20s |\n",
                flights[i].currentCode, flights[i].altitude, flights[i].speed,
                flights[i].destinationCode, flights[i].remainingFuel, "Sufficient Fuel", flights[i].weatherCondition);
        }
    }
}

// Function to free memory allocated for the graph
void freegraph(Graph* graph) {
    for (int i = 0; i < graph->numNodes; ++i) {
        Node* current = graph->adjList[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->adjList);
}

// Main function
int main() {
    int numAirports;
    printf("Enter the number of airports: ");
    scanf("%d", &numAirports);

    char** airportCodes = (char*)malloc(numAirports * sizeof(char));
    for (int i = 0; i < numAirports; ++i) {
        airportCodes[i] = (char*)malloc((MAX_CODE_LENGTH + 1) * sizeof(char));
    }

    printf("Enter airport codes for %d airports (e.g., BLR, JFK, LHR, DEL):\n", numAirports);
    for (int i = 0; i < numAirports; ++i) {
        printf("Airport %d: ", i + 1);
        scanf("%s", airportCodes[i]);
    }

    Graph airspace = create(numAirports, airportCodes);

    // Add edges between airports (flights can travel between any pair of airports)
    for (int i = 0; i < numAirports; ++i) {
        for (int j = i + 1; j < numAirports; ++j) {
            add(&airspace, i, j);
        }
    }

    int numFlights;
    printf("Enter the number of flights: ");
    scanf("%d", &numFlights);
    Flight* flights = (Flight*)malloc(numFlights * sizeof(Flight));
    for (int i = 0; i < numFlights; ++i) {
        printf("Enter data for Flight %d:\n", i + 1);
        flights[i].id = i + 1;
        printf("Departure From (Airport code): ");
        scanf("%s", flights[i].currentCode);
        flights[i].altitude = 10000.0;
        flights[i].speed = 500.0;
        printf("Arrival At (Airport code): ");
        scanf("%s", flights[i].destinationCode);
        printf("Remaining Fuel: ");
        scanf("%d", &flights[i].remainingFuel);
        printf("Weather Condition (good/bad): ");
        scanf("%s", flights[i].weatherCondition);
    }

    collision(flights, numFlights);

    // Check fuel sufficiency and find the nearest node if there is a shortage
    findclosest(flights, numFlights, &airspace);

    // Clean up allocated memory
    free(flights);
    for (int i = 0; i < numAirports; ++i) {
        free(airportCodes[i]);
    }
    free(airportCodes);

    // Clean up graph memory
    freegraph(&airspace);

    return 0;
}