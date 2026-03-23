#ifndef HEAP_H
#define HEAP_H

#include "structs.h" // <-- FIX: Includes the definition of MaxHeap and Product

// Function Prototypes
MaxHeap* createMaxHeap(int capacity);
void swapProduct(Product* a, Product* b);
void maxHeapify(MaxHeap* heap, int idx);
void insertProduct(MaxHeap* heap, Product product);
Product extractMaxPriorityProduct(MaxHeap* heap);
void printHeap(MaxHeap* heap); // This is for debugging

#endif // HEAP_H