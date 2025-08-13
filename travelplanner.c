#include "travelplanner.h"

static TravelPlanner travelPlannerInstance;
static int initialized = 0;

TravelPlanner* getTravelPlanner() {
    if (!initialized) {
        travelPlannerInstance.n = 0;
        travelPlannerInstance.edgeCount = 0;
        memset(travelPlannerInstance.adjCount, 0, sizeof(travelPlannerInstance.adjCount));
        initialized = 1;
    }
    return &travelPlannerInstance;
}

void loadFromFile(TravelPlanner* tp, const char* filename, const char* selectedCity) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Could not open file %s.\n", filename);
        return;
    }

    tp->n = 0;
    tp->edgeCount = 0;
    memset(tp->adjCount, 0, sizeof(tp->adjCount));

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char city[MAX_NAME_LEN], src[MAX_NAME_LEN], dst[MAX_NAME_LEN], distStr[MAX_NAME_LEN];
        if (sscanf(line, "%[^,],%[^,],%[^,],%s", city, src, dst, distStr) == 4) {
            if (strcmp(city, selectedCity) != 0) continue;
            strcpy(tp->routes[tp->edgeCount][0], city);
            strcpy(tp->routes[tp->edgeCount][1], src);
            strcpy(tp->routes[tp->edgeCount][2], dst);
            strcpy(tp->routes[tp->edgeCount][3], distStr);

            // Add unique locations
            int foundSrc = 0, foundDst = 0;
            for (int i = 0; i < tp->n; i++) {
                if (strcmp(tp->locations[i], src) == 0) foundSrc = 1;
                if (strcmp(tp->locations[i], dst) == 0) foundDst = 1;
            }
            if (!foundSrc) strcpy(tp->locations[tp->n++], src);
            if (!foundDst) strcpy(tp->locations[tp->n++], dst);

            tp->edgeCount++;
        }
    }

    fclose(file);
    printf("Loaded data for city: %s\n", selectedCity);
}

void inputFromUser(TravelPlanner* tp) {
    printf("Enter city name: ");
    fgets(tp->city, MAX_NAME_LEN, stdin);
    tp->city[strcspn(tp->city, "\n")] = 0;

    printf("Enter number of locations: ");
    scanf("%d", &tp->n);
    getchar();
    for (int i = 0; i < tp->n; i++) {
        printf("Enter location %d: ", i + 1);
        fgets(tp->locations[i], MAX_NAME_LEN, stdin);
        tp->locations[i][strcspn(tp->locations[i], "\n")] = 0;
    }

    printf("Enter number of routes: ");
    scanf("%d", &tp->edgeCount);
    getchar();
    for (int i = 0; i < tp->edgeCount; i++) {
        printf("Enter route %d (source destination distance): ", i + 1);
        char src[MAX_NAME_LEN], dst[MAX_NAME_LEN], dist[MAX_NAME_LEN];
        scanf("%s %s %s", src, dst, dist);
        strcpy(tp->routes[i][0], tp->city);
        strcpy(tp->routes[i][1], src);
        strcpy(tp->routes[i][2], dst);
        strcpy(tp->routes[i][3], dist);
    }
}

int findLocationIndex(TravelPlanner* tp, const char* name) {
    for (int i = 0; i < tp->n; i++) {
        if (strcasecmp(name, tp->locations[i]) == 0) return i;
    }
    return -1;
}

void displayLocations(TravelPlanner* tp) {
    printf("--------------- Locations ---------------\n");
    for (int i = 0; i < tp->n; i++) {
        printf("%s\n", tp->locations[i]);
    }
    printf("----------------------------------------\n");
}

void displayRoutes(TravelPlanner* tp) {
    printf("--------- Routes (City: %s) ---------\n", tp->city);
    for (int i = 0; i < tp->edgeCount; i++) {
        printf("%s to %s takes %s kms\n", tp->routes[i][1], tp->routes[i][2], tp->routes[i][3]);
    }
    printf("----------------------------------------\n");
}

void createEdgeVector(TravelPlanner* tp) {
    for (int i = 0; i < tp->edgeCount; i++) {
        int sourceIndex = findLocationIndex(tp, tp->routes[i][1]);
        int destIndex = findLocationIndex(tp, tp->routes[i][2]);
        double dist = atof(tp->routes[i][3]);
        tp->edges[i][0] = sourceIndex;
        tp->edges[i][1] = destIndex;
        tp->edges[i][2] = dist;
    }
}
// increment neighbour count And Add each Destination and distance to source
void createAdjacencyMatrix(TravelPlanner* tp) {
    for (int i = 0; i < tp->edgeCount; i++) {
        int src = (int)tp->edges[i][0];
        int dest = (int)tp->edges[i][1];
        double dist = tp->edges[i][2];
        tp->adj[src][tp->adjCount[src]].dest = dest;
        tp->adj[src][tp->adjCount[src]].weight = dist;
        tp->adjCount[src]++;
    }
}

void shortestPathWithMinimumStops(TravelPlanner* tp, char* src, char* dst, double farePerKm) {
    int source = findLocationIndex(tp, src);
    int dest = findLocationIndex(tp, dst);
    if (source == -1 || dest == -1) {
        printf("Invalid source or destination.\n");
        return;
    }

    double distance[MAX_LOCATIONS];
    int parent[MAX_LOCATIONS];
    int stops[MAX_LOCATIONS];
    int visited[MAX_LOCATIONS] = {0};

    for (int i = 0; i < tp->n; i++) {
        distance[i] = INF;
        parent[i] = -1;
        stops[i] = INF;
    }

    distance[source] = 0;
    parent[source] = source;
    stops[source] = 0;

    // used dijkstra algorithm here 

    for (int count = 0; count < tp->n; count++) {
        int u = -1;
        for (int i = 0; i < tp->n; i++) {
            if (!visited[i] && (u == -1 || distance[i] < distance[u])) u = i;
        }

        if (distance[u] == INF) break;
        visited[u] = 1;


    // core traversal logic of dijsktra and the destination gives connected node input and weight gives distance to that node
        for (int i = 0; i < tp->adjCount[u]; i++) {
            int v = tp->adj[u][i].dest;
            double w = tp->adj[u][i].weight;

            if (distance[v] > distance[u] + w) {
                distance[v] = distance[u] + w;
                parent[v] = u;
                stops[v] = stops[u] + 1;
            } else if (distance[v] == distance[u] + w && stops[v] > stops[u] + 1) {
                parent[v] = u;
                stops[v] = stops[u] + 1;
            }
        }
    }

    if (distance[dest] == INF) {
        printf("No route available.\n");
        return;
    }

    int path[MAX_LOCATIONS];
    int len = 0;
    for (int v = dest; v != source; v = parent[v]) path[len++] = v;
    path[len++] = source;

    printf("Total Distance: %.2lf kms\n", distance[dest]);
    printf("Estimated Cost: ₹%.2lf\n", distance[dest] * farePerKm);
    printf("Stops: %d\n", stops[dest] > 0 ? stops[dest] - 1 : 0);
    printf("Path: ");
    for (int i = len - 1; i >= 0; i--) {
        printf("%s", tp->locations[path[i]]);
        if (i > 0) printf(" -> ");
        else printf(".\n");
    }
}

int main() {
    TravelPlanner* tp = getTravelPlanner();
    char method;

    printf("Welcome to the Multi-City Travel Planner!\n");
    printf("Do you want to load data from a file (F) or enter manually (M)? ");
    scanf(" %c", &method);
    getchar(); // consume newline

    if (method == 'F' || method == 'f') {
        char filename[MAX_NAME_LEN], selectedCity[MAX_NAME_LEN];
        printf("Enter file name (e.g., routes.csv): ");
        fgets(filename, MAX_NAME_LEN, stdin);
        filename[strcspn(filename, "\n")] = 0;

        printf("Enter the city name to load routes for: ");
        fgets(selectedCity, MAX_NAME_LEN, stdin);
        selectedCity[strcspn(selectedCity, "\n")] = 0;

        strcpy(tp->city, selectedCity);
        loadFromFile(tp, filename, selectedCity);
    } else {
        inputFromUser(tp);
    }

    createEdgeVector(tp);
    createAdjacencyMatrix(tp);
    displayLocations(tp);
    displayRoutes(tp);

    double farePerKm = 10.0;
    printf("\nEnter fare per kilometer (₹): ");
    scanf("%lf", &farePerKm);
    getchar();

    char prompt = 'Y';
    while (prompt == 'Y' || prompt == 'y') {
        char source[MAX_NAME_LEN], destination[MAX_NAME_LEN];
        printf("\nEnter source location: ");
        fgets(source, MAX_NAME_LEN, stdin);
        source[strcspn(source, "\n")] = 0;

        printf("Enter destination location: ");
        fgets(destination, MAX_NAME_LEN, stdin);
        destination[strcspn(destination, "\n")] = 0;

        shortestPathWithMinimumStops(tp, source, destination, farePerKm);

        printf("\nDo you want to find another route? (Y/N): ");
        scanf(" %c", &prompt);
        getchar();
    }

    printf("\nThank you for using the Travel Planner!\n");
    return 0;
}