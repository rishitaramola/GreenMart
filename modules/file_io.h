#ifndef FILE_IO_H
#define FILE_IO_H

#include "structs.h" // <-- FIX: Includes all struct definitions

// --- THIS IS THE FIX ---
// These definitions were missing from file_io.c
#define PRODUCT_FILE "data/products.dat"
#define REQUEST_FILE "data/requests.dat"
#define USER_FILE "data/users.dat"

// Function Prototypes
void saveProducts(MaxHeap* heap);
void loadProducts(MaxHeap* heap);

void saveRequests(LinkedList* list);
void loadRequests(LinkedList* list);

void saveUsers(HashTable* table);
void loadUsers(HashTable* table);

#endif // FILE_IO_H