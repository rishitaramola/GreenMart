#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "structs.h" // <-- FIX: Includes definition of HashTable, Node, etc.

// Function Prototypes
unsigned int hash(const char *key);
HashTable* createHashTable();
void insert(HashTable* table, const char *key, const char *value);
const char* search(HashTable* table, const char *key);
void deleteKey(HashTable* table, const char *key);
void printHashTable(HashTable* table);

#endif // HASH_TABLE_H