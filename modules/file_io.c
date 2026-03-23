#include "file_io.h" // <-- FIX: Includes defines like REQUEST_FILE
#include "heap.h" 
#include "linked_list.h" 
#include "hash_table.h" // <-- FIX: Added this include
#include <stdlib.h>
#include <string.h>

// --- THIS IS THE FIX ---
#include <direct.h> // For _mkdir

// --- Helper function to create the data directory ---
void ensureDataDirectory() {
    _mkdir("data");
}

// --- Product Saving/Loading (Heap) ---
void saveProducts(MaxHeap* heap) {
    ensureDataDirectory();
    FILE *fp = fopen(PRODUCT_FILE, "wb");
    if (fp == NULL) { return; }
    fwrite(&(heap->size), sizeof(int), 1, fp);
    if (heap->size > 0) { 
        fwrite(heap->array, sizeof(Product), heap->size, fp); 
    }
    fclose(fp);
}

void loadProducts(MaxHeap* heap) {
    FILE *fp = fopen(PRODUCT_FILE, "rb");
    if (fp == NULL) { return; }
    int loaded_size;
    if (fread(&loaded_size, sizeof(int), 1, fp) != 1) { fclose(fp); return; }
    if (loaded_size > 0 && loaded_size <= heap->capacity) {
        heap->size = 0; 
        Product temp_product;
        for (int i = 0; i < loaded_size; i++) {
            if (fread(&temp_product, sizeof(Product), 1, fp) == 1) {
                insertProduct(heap, temp_product); 
            }
        }
    }
    fclose(fp);
}

// --- Request Saving/Loading (Linked List) ---
void saveRequests(LinkedList* list) {
    // --- THIS IS THE TYPO FIX ---
    ensureDataDirectory(); // Was 'ensureDataDriectory'
    
    FILE *fp = fopen(REQUEST_FILE, "wb");
    if (fp == NULL) { return; }
    fwrite(&(list->size), sizeof(int), 1, fp);
    LLNode* current = list->head;
    while(current) {
        fwrite(&(current->data), sizeof(Request), 1, fp);
        current = current->next;
    }
    fclose(fp);
}

void loadRequests(LinkedList* list) {
    FILE *fp = fopen(REQUEST_FILE, "rb");
    if (fp == NULL) { return; }
    int loaded_size;
    if (fread(&loaded_size, sizeof(int), 1, fp) != 1) { fclose(fp); return; }
    Request temp_req;
    for (int i = 0; i < loaded_size; i++) {
        if (fread(&temp_req, sizeof(Request), 1, fp) == 1) {
            addRequest(list, temp_req);
        }
    }
    fclose(fp);
}

// --- User Saving/Loading (Hash Table) ---
void saveUsers(HashTable* table) {
    ensureDataDirectory();
    FILE *fp = fopen(USER_FILE, "wb");
    if (fp == NULL) { return; }
    int count = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* current = table->array[i];
        while (current != NULL) {
            count++;
            current = current->next;
        }
    }
    fwrite(&count, sizeof(int), 1, fp);
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* current = table->array[i];
        while (current != NULL) {
            fwrite(&current->data, sizeof(KeyValuePair), 1, fp);
            current = current->next;
        }
    }
    fclose(fp);
}

void loadUsers(HashTable* table) {
    FILE *fp = fopen(USER_FILE, "rb");
    if (fp == NULL) { return; }
    int loaded_count = 0;
    if (fread(&loaded_count, sizeof(int), 1, fp) != 1) { fclose(fp); return; }
    KeyValuePair kvp;
    for (int i = 0; i < loaded_count; i++) {
        if (fread(&kvp, sizeof(KeyValuePair), 1, fp) == 1) {
            insert(table, kvp.key, kvp.value); 
        }
    }
    fclose(fp);
}