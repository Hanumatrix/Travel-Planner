
#ifndef TRAVELPLANNER_H
#define TRAVELPLANNER_H

#define MAX_LOCATIONS 100
#define MAX_ROUTES 200
#define MAX_NAME_LEN 100
#define MAX_EDGES 200
#define INF 1e9

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
    int n;
    char city[MAX_NAME_LEN];
    char locations[MAX_LOCATIONS][MAX_NAME_LEN];
    char routes[MAX_ROUTES][4][MAX_NAME_LEN];

    char indexToLocation[MAX_LOCATIONS][MAX_NAME_LEN];
    char locationToDisplayName[MAX_LOCATIONS][MAX_NAME_LEN];

    double edges[MAX_EDGES][3];
    int edgeCount;

    // implementation of adjacent list
    struct {
        int dest;
        double weight;
    } adj[MAX_LOCATIONS][MAX_LOCATIONS];
    int adjCount[MAX_LOCATIONS];
} TravelPlanner;

TravelPlanner* getTravelPlanner();
void loadFromFile(TravelPlanner* tp, const char* filename, const char* selectedCity);
void inputFromUser(TravelPlanner* tp);
void displayLocations(TravelPlanner* tp);
void displayRoutes(TravelPlanner* tp);
void createEdgeVector(TravelPlanner* tp);
void createAdjacencyMatrix(TravelPlanner* tp);
void shortestPathWithMinimumStops(TravelPlanner* tp, char* src, char* dst, double farePerKm);
int findLocationIndex(TravelPlanner* tp, const char* name);

#endif