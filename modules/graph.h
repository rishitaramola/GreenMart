#ifndef GRAPH_H
#define GRAPH_H

#include "structs.h" // <-- FIX: Includes definition of Graph, AdjNode, etc.
#include <limits.h>  // For INT_MAX

// Function Prototypes
Graph* createGraph(int num_nodes);
void addNodeName(Graph* graph, int index, const char* name);
void addEdge(Graph* graph, int src, int dest, int weight);

// --- THIS IS THE FIX ---
// The prototype now matches the implementation in graph.c
void dijkstra(Graph* graph, int start_node, int dist[], int parent[]);

#endif // GRAPH_H