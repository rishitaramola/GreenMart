#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "structs.h" // <-- FIX: Includes definition of LinkedList, Request, etc.

// Function Prototypes
LinkedList* createLinkedList();
void addRequest(LinkedList* list, Request new_req);
int updateRequestStatus(LinkedList* list, const char* req_id, const char* new_status);
LLNode* findRequest(LinkedList* list, const char* product_name);
int cancelRequest(LinkedList* list, const char* req_id);
void printLinkedList(LinkedList* list, const char* list_name); // For debugging

#endif // LINKED_LIST_H