#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>

/* * This is the central file for all data structures.
 * It defines what 'MaxHeap', 'Product', 'HashTable', etc., are.
 * ALL OTHER .h FILES WILL INCLUDE THIS.
 */

// 1. Product (for the Heap)
typedef struct {
    char product_name[50];
    char farmer_id[20];
    float quantity; // in Kg
    float price_per_kg; // For billing
    int freshness_score; // Days remaining until expiry
} Product;

// 2. MaxHeap (for the Inventory)
typedef struct {
    Product *array;
    int capacity;
    int size;
} MaxHeap;

// 3. KeyValuePair (for the Hash Table)
#define MAX_VALUE_LEN 256 // Must match hash_table.h
typedef struct {
    char key[50];
    char value[MAX_VALUE_LEN]; // For user data "Name|Contact|Type|Password"
} KeyValuePair;

// 4. Hash Table Node
typedef struct Node {
    KeyValuePair data;
    struct Node* next;
} Node;

// 5. Hash Table (for Users)
#define TABLE_SIZE 101 // Must match hash_table.c
typedef struct {
    Node* array[TABLE_SIZE]; 
} HashTable;

// 6. Request (for the Linked List)
typedef struct Request {
    char request_id[20];
    char buyer_id[20];
    char product_name[50];
    float quantity;
    char status[30]; // PENDING, ALLOCATED, CANCELED
} Request;

// 7. Linked List Node
typedef struct LLNode {
    Request data;
    struct LLNode* next;
    struct LLNode* prev;
} LLNode;

// 8. Linked List (for Requests/History)
typedef struct {
    LLNode* head;
    LLNode* tail;
    int size;
} LinkedList;

// 9. Graph (for Logistics)
#define MAX_NODES 50
typedef struct {
    int dest;
    int weight;
} Edge;

typedef struct AdjNode {
    Edge edge;
    struct AdjNode* next;
} AdjNode;

typedef struct {
    int num_nodes;
    AdjNode* adj_list[MAX_NODES];
    char node_names[MAX_NODES][50];
} Graph;

#endif // STRUCTS_H