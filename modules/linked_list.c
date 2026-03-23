#include "linked_list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

LinkedList* createLinkedList() {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    if (list == NULL) return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void addRequest(LinkedList* list, Request new_req) {
    LLNode* new_node = (LLNode*)malloc(sizeof(LLNode));
    if (new_node == NULL) return;
    new_node->data = new_req;
    new_node->next = NULL;
    new_node->prev = list->tail;
    if (list->tail == NULL) {
        list->head = new_node;
    } else {
        list->tail->next = new_node;
    }
    list->tail = new_node;
    list->size++;
}

int updateRequestStatus(LinkedList* list, const char* req_id, const char* new_status) {
    LLNode* current = list->head;
    while (current != NULL) {
        if (strcmp(current->data.request_id, req_id) == 0) {
            strncpy(current->data.status, new_status, 29);
            current->data.status[29] = '\0';
            return 1; // Success
        }
        current = current->next;
    }
    return 0; // Not found
}

// --- This function is for the Allocation Engine ---
LLNode* findRequest(LinkedList* list, const char* product_name) {
    LLNode* current = list->head;
    while (current != NULL) {
        if (strcmp(current->data.product_name, product_name) == 0 &&
            strcmp(current->data.status, "PENDING") == 0) {
            return current; // Found a match
        }
        current = current->next;
    }
    return NULL; // No pending request
}

int cancelRequest(LinkedList* list, const char* req_id) {
    LLNode* current = list->head;
    while (current != NULL && strcmp(current->data.request_id, req_id) != 0) {
        current = current->next;
    }
    if (current == NULL) return 0; 
    if (current->prev != NULL) current->prev->next = current->next;
    else list->head = current->next;
    if (current->next != NULL) current->next->prev = current->prev;
    else list->tail = current->prev;
    free(current);
    list->size--;
    return 1;
}

void printLinkedList(LinkedList* list, const char* list_name) {
    // ... (This function is for debugging) ...
}