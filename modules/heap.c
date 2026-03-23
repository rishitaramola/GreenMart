#include "heap.h"
#include <stdlib.h>
#include <stdio.h> // For printHeap

MaxHeap* createMaxHeap(int capacity) {
    MaxHeap* heap = (MaxHeap*)malloc(sizeof(MaxHeap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (Product*)malloc(capacity * sizeof(Product));
    return heap;
}

void swapProduct(Product* a, Product* b) {
    Product temp = *a;
    *a = *b;
    *b = temp;
}

void maxHeapify(MaxHeap* heap, int idx) {
    int largest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    // Min-Heap logic based on freshness_score (lower score = higher priority)
    if (left < heap->size && heap->array[left].freshness_score < heap->array[largest].freshness_score)
        largest = left;
    if (right < heap->size && heap->array[right].freshness_score < heap->array[largest].freshness_score)
        largest = right;

    if (largest != idx) {
        swapProduct(&heap->array[idx], &heap->array[largest]);
        maxHeapify(heap, largest);
    }
}

void insertProduct(MaxHeap* heap, Product product) {
    if (heap->size == heap->capacity) {
        // Cannot print in a server engine
        return;
    }
    int i = heap->size++;
    heap->array[i] = product;
    while (i != 0 && heap->array[i].freshness_score < heap->array[(i - 1) / 2].freshness_score) {
        swapProduct(&heap->array[i], &heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

Product extractMaxPriorityProduct(MaxHeap* heap) {
    if (heap->size <= 0) {
        Product empty = {"N/A", "N/A", 0.0, 0.0, 9999}; 
        return empty;
    }
    if (heap->size == 1) {
        heap->size--;
        return heap->array[0];
    }
    Product root = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;
    maxHeapify(heap, 0);
    return root;
}

void printHeap(MaxHeap* heap) {
    printf("\n--- Freshness Priority Queue (Product Inventory) ---\n");
    if (heap->size == 0) {
        printf("Inventory is empty.\n");
        return;
    }
    // ... (rest of print function)
}