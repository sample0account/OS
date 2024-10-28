#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxnodes 10
#define infinity 99999

struct Node {
    int id;
    int cost[maxnodes];
    int nexthop[maxnodes];
};

int n;
int costmat[maxnodes][maxnodes];
struct Node nodes[maxnodes];

void init() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                nodes[i].cost[j] = 0;
                nodes[i].nexthop[j] = i;
            } else if (costmat[i][j] != -1) {
                nodes[i].cost[j] = costmat[i][j];
                nodes[i].nexthop[j] = j;
            } else {
                nodes[i].cost[j] = infinity;
                nodes[i].nexthop[j] = -1;
            }
        }
    }
}

void printTable() {
    for (int i = 0; i < n; i++) {
        printf("Routing Table for the node %d\n", i);
        printf("Destination\tDistance\tNext Hop\n");

        for (int j = 0; j < n; j++) {
            if (nodes[i].cost[j] == infinity) {
                printf("%d\t\tinfinity\t-\n", j);
            } else {
                printf("%d\t\t%d\t\t%d\n", j, nodes[i].cost[j], nodes[i].nexthop[j]);
            }
        }
        printf("\n");
    }
}

int updateRoutingTable() {
    int updated = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                if (nodes[i].cost[k] != infinity && nodes[k].cost[j] != infinity &&
                    nodes[i].cost[k] + nodes[k].cost[j] < nodes[i].cost[j]) {
                    nodes[i].cost[j] = nodes[i].cost[k] + nodes[k].cost[j];
                    nodes[i].nexthop[j] = nodes[i].nexthop[k];
                    updated = 1;
                }
            }
        }
    }
    return updated;
}

int main() {
    printf("Enter the number of nodes: ");
    if (scanf("%d", &n) != 1 || n < 1 || n > maxnodes) {
        fprintf(stderr, "Invalid number of nodes.\n");
        return 1;
    }

    printf("Enter the cost matrix (enter -1 for unreachable paths):\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (scanf("%d", &costmat[i][j]) != 1) {
                fprintf(stderr, "Invalid input for cost matrix.\n");
                return 1;
            }
        }
    }

    init();
    printTable();

    int updated;
    do {
        updated = updateRoutingTable();
    } while (updated);

    printf("Routing tables have converged.\n");
    printTable();

    return 0;
}
