#include "graph.h"
#include <stdlib.h>
#include <string.h> 
#include <stdio.h>  

// Helper function
int minDistance(int dist[], int spt_set[], int num_nodes) {
    int min = INT_MAX, min_index = -1;
    for (int v = 0; v < num_nodes; v++)
        if (spt_set[v] == 0 && dist[v] <= min)
            min = dist[v], min_index = v;
    return min_index;
}

Graph* createGraph(int num_nodes) {
    if (num_nodes > MAX_NODES) return NULL;
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (graph == NULL) return NULL;
    graph->num_nodes = num_nodes;
    for (int i = 0; i < num_nodes; i++) {
        graph->adj_list[i] = NULL;
        strcpy(graph->node_names[i], "Unnamed");
    }
    return graph;
}

void addNodeName(Graph* graph, int index, const char* name) {
    if (index >= 0 && index < graph->num_nodes) {
        strncpy(graph->node_names[index], name, 49);
        graph->node_names[index][49] = '\0';
    }
}

void addEdge(Graph* graph, int src, int dest, int weight) {
    AdjNode* new_node = (AdjNode*)malloc(sizeof(AdjNode));
    if(new_node == NULL) return;
    new_node->edge.dest = dest;
    new_node->edge.weight = weight;
    new_node->next = graph->adj_list[src];
    graph->adj_list[src] = new_node;

    AdjNode* new_node_rev = (AdjNode*)malloc(sizeof(AdjNode));
    if(new_node_rev == NULL) return;
    new_node_rev->edge.dest = src;
    new_node_rev->edge.weight = weight;
    new_node_rev->next = graph->adj_list[dest];
    graph->adj_list[dest] = new_node_rev;
}

// --- THIS IS THE FIX ---
// This is the 4-argument "silent" version
void dijkstra(Graph* graph, int start_node, int dist[], int parent[]) {
    int spt_set[MAX_NODES]; 
    for (int i = 0; i < graph->num_nodes; i++) {
        dist[i] = INT_MAX;
        spt_set[i] = 0;
        parent[i] = -1;
    }
    dist[start_node] = 0;

    for (int count = 0; count < graph->num_nodes - 1; count++) {
        int u = minDistance(dist, spt_set, graph->num_nodes);
        if (u == -1) break; 
        spt_set[u] = 1; 

        AdjNode* current = graph->adj_list[u];
        while (current != NULL) {
            int v = current->edge.dest;
            int weight = current->edge.weight;
            
            if (!spt_set[v] && dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
            }
            current = current->next;
        }
    }
}