#include "hash_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

// DJB2 hash function
unsigned int hash(const char *key) {
    unsigned long hash_val = 5381;
    int c;
    while ((c = *key++))
        hash_val = ((hash_val << 5) + hash_val) + c; 
    return hash_val % TABLE_SIZE;
}

HashTable* createHashTable() {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if(table == NULL) return NULL;
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->array[i] = NULL;
    }
    return table;
}

void insert(HashTable* table, const char *key, const char *value) {
    unsigned int index = hash(key);
    
    Node* current = table->array[index];
    while(current) {
        if (strcmp(current->data.key, key) == 0) {
            strncpy(current->data.value, value, MAX_VALUE_LEN - 1);
            current->data.value[MAX_VALUE_LEN - 1] = '\0';
            return;
        }
        current = current->next;
    }

    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) return; 
    strncpy(new_node->data.key, key, 49);
    new_node->data.key[49] = '\0';
    strncpy(new_node->data.value, value, MAX_VALUE_LEN - 1);
    new_node->data.value[MAX_VALUE_LEN - 1] = '\0';
    
    new_node->next = table->array[index];
    table->array[index] = new_node;
}

const char* search(HashTable* table, const char *key) {
    unsigned int index = hash(key);
    Node* current = table->array[index];
    while (current != NULL) {
        if (strcmp(current->data.key, key) == 0) {
            return current->data.value; // Found
        }
        current = current->next;
    }
    return NULL; // Not found
}

void deleteKey(HashTable* table, const char *key) { /* Not needed for this project */ }
void printHashTable(HashTable* table) { /* Not needed for this project */ }