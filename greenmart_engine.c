/* --- GreenMart C Engine (Backend) --- */
/*
 * This is the NON-INTERACTIVE backend engine for the
 * "Freshness-First" (Heap-based) model.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

// --- Includes for Data Structures ---
#include "modules/structs.h" 
#include "modules/file_io.h" 
#include "modules/heap.h"
#include "modules/hash_table.h"
#include "modules/linked_list.h"
#include "modules/graph.h" 

#define MAX_VALUE_LEN 256 
#define PERISHABLE_DAYS_THRESHOLD 30

// --- Global Data Structures ---
MaxHeap* G_product_heap;
HashTable* G_user_table;
LinkedList* G_request_list;
Graph* G_market_graph;

// --- Engine Helper Functions ---
void initialize_engine_data() {
    G_product_heap = createMaxHeap(500); 
    G_user_table = createHashTable();
    G_request_list = createLinkedList();
    G_market_graph = createGraph(50); 
    
    // Load persisted data from files
    loadProducts(G_product_heap); 
    loadRequests(G_request_list);  
    loadUsers(G_user_table); 

    // Insert defaults if necessary
    if (search(G_user_table, "Rishita") == NULL) {
        insert(G_user_table, "Rishita", "Rishita_Ramola|rishita@g.com|F|greenpass1"); 
    }
    if (search(G_user_table, "Tanisha") == NULL) {
        insert(G_user_table, "Tanisha", "Tanisha_Kapoor|tanisha@g.com|B|martpass2");
    }
    
    // Graph setup...
    addNodeName(G_market_graph, 0, "Farmer (Source)");
    addNodeName(G_market_graph, 1, "Main Market Hub");
    addNodeName(G_market_graph, 2, "Buyer: Tanisha");
    addNodeName(G_market_graph, 3, "Buyer: Siddhi");
    addNodeName(G_market_graph, 4, "Cold Storage");
    
    addEdge(G_market_graph, 0, 1, 15); // Farmer -> Hub
    addEdge(G_market_graph, 1, 2, 10); // Hub -> Tanisha
    addEdge(G_market_graph, 1, 3, 20); // Hub -> Siddhi
    addEdge(G_market_graph, 0, 4, 5);  // Farmer -> Cold Storage
    addEdge(G_market_graph, 4, 3, 10); // Cold Storage -> Siddhi
}

void shutdown_engine_data() {
    saveProducts(G_product_heap); 
    saveRequests(G_request_list);  
    saveUsers(G_user_table); 
}

// 1. ENGINE COMMAND: Login
void engine_login(const char* user_id, const char* password) {
    const char *user_data = search(G_user_table, user_id);
    if (user_data == NULL) {
        printf("{\"status\":\"error\", \"message\":\"User not found\"}");
        return;
    }
    char data_copy[MAX_VALUE_LEN]; 
    strncpy(data_copy, user_data, MAX_VALUE_LEN - 1);
    data_copy[MAX_VALUE_LEN - 1] = '\0';
    char *name = strtok(data_copy, "|");
    strtok(NULL, "|"); // Skip contact
    char *type_str = strtok(NULL, "|");
    char *stored_password = strtok(NULL, "|");
    if (stored_password && strcmp(password, stored_password) == 0) {
        printf("{\"status\":\"success\", \"user_type\":\"%s\", \"name\":\"%s\"}", type_str, name);
    } else {
        printf("{\"status\":\"error\", \"message\":\"Incorrect password\"}");
    }
}

// 2. ENGINE COMMAND: Register
void engine_register_user(const char* user_id, const char* name, const char* contact, const char* type, const char* password) {
    if (search(G_user_table, user_id) != NULL) {
        printf("{\"status\":\"error\", \"message\":\"User ID '%s' is already taken.\"}", user_id);
        return;
    }
    if (type[0] != 'F' && type[0] != 'B') {
        printf("{\"status\":\"error\", \"message\":\"Invalid user type. Must be 'F' or 'B'.\"}");
        return;
    }
    char user_data[MAX_VALUE_LEN];
    snprintf(user_data, MAX_VALUE_LEN, "%s|%s|%s|%s", name, contact, type, password);
    insert(G_user_table, user_id, user_data);
    printf("{\"status\":\"success\", \"message\":\"User '%s' registered successfully.\"}", user_id);
}

// 3. ENGINE COMMAND: Add Product
void engine_add_product(const char* name, float qty, float price, int expiry, const char* farmer_id) {
    if (qty <= 0 || price <= 0 || expiry <= 0) {
        printf("{\"status\":\"error\", \"message\":\"Validation Failed: All numbers must be positive.\"}");
        return;
    }
    Product p;
    strncpy(p.product_name, name, 49); p.product_name[49] = '\0';
    strncpy(p.farmer_id, farmer_id, 19); p.farmer_id[19] = '\0';
    p.quantity = qty;
    p.price_per_kg = price; 
    p.freshness_score = expiry;
    insertProduct(G_product_heap, p);
    printf("{\"status\":\"success\", \"message\":\"Product %s added to inventory.\"}", name);
}

// 4. ENGINE COMMAND: Add Request (UPDATED: Instant Allocation)
void engine_add_request(const char* buyer_id, const char* product_name, float qty) {
    if (qty <= 0) {
         printf("{\"status\":\"error\", \"message\":\"Validation Failed: Quantity must be positive.\"}");
         return;
    }
    
    // --- Try to find this product in the Heap (O(n) search) ---
    int found_index = -1;
    for (int i = 0; i < G_product_heap->size; i++) {
        if (strcmp(G_product_heap->array[i].product_name, product_name) == 0) {
            found_index = i;
            break;
        }
    }

    // --- Case 1: Product NOT found in Heap ---
    if (found_index == -1) {
        Request req;
        strncpy(req.buyer_id, buyer_id, 19); req.buyer_id[19] = '\0';
        strncpy(req.product_name, product_name, 49); req.product_name[49] = '\0';
        req.quantity = qty;
        sprintf(req.request_id, "REQ%04d", G_request_list->size + 1);
        strncpy(req.status, "PENDING", 29); req.status[29] = '\0';
        addRequest(G_request_list, req);
        
        printf("{\"status\":\"warn\", \"message\":\"Out of Stock: Request PENDING until farmers add more.\"}");
        return;
    }

    // --- Case 2: Product Found (Allocate Instantly) ---
    Product* p = &G_product_heap->array[found_index];
    
    if (p->quantity >= qty) {
        // We have enough stock -> ALLOCATE IT NOW
        float remaining_qty = p->quantity - qty;
        float price_per_kg = p->price_per_kg;
        float total_cost = qty * price_per_kg;

        Request req;
        strncpy(req.buyer_id, buyer_id, 19); req.buyer_id[19] = '\0';
        strncpy(req.product_name, product_name, 49); req.product_name[49] = '\0';
        req.quantity = qty;
        sprintf(req.request_id, "REQ%04d", G_request_list->size + 1);
        strncpy(req.status, "ALLOCATED", 29); req.status[29] = '\0';
        addRequest(G_request_list, req);
        
        // Update Inventory
        if (remaining_qty > 0) {
            p->quantity = remaining_qty;
        } else {
            // Remove product if qty becomes 0
            G_product_heap->array[found_index] = G_product_heap->array[G_product_heap->size - 1];
            G_product_heap->size--;
            maxHeapify(G_product_heap, found_index); 
        }

        // Return Bill
        printf("{\"status\":\"success\", "
               "\"message\":\"Item Allocated Successfully!\", "
               "\"bill\": {"
                   "\"req_id\":\"%s\", "
                   "\"buyer_id\":\"%s\", "
                   "\"product\":\"%s\", "
                   "\"farmer_id\":\"%s\", "
                   "\"qty_kg\":%.2f, "
                   "\"price_per_kg\":%.2f, "
                   "\"total_cost\":%.2f"
               "}}",
               req.request_id, req.buyer_id, p->product_name, p->farmer_id, qty, price_per_kg, total_cost);
    } else {
        // --- Case 3: Not Enough Stock ---
        // Create a PENDING request for the backorder
        Request req;
        strncpy(req.buyer_id, buyer_id, 19); req.buyer_id[19] = '\0';
        strncpy(req.product_name, product_name, 49); req.product_name[49] = '\0';
        req.quantity = qty;
        sprintf(req.request_id, "REQ%04d", G_request_list->size + 1);
        strncpy(req.status, "PENDING", 29); req.status[29] = '\0';
        addRequest(G_request_list, req);

        printf("{\"status\":\"warn\", \"message\":\"Insufficient Stock (Only %.2f kg available). Request set to PENDING.\"}", p->quantity);
    }
}

// --- 5. ENGINE COMMAND: Run Allocation (UPDATED: Process All Items) ---
void engine_run_allocation() {
    if (G_product_heap->size == 0) {
        printf("{\"status\":\"error\", \"message\":\"Allocation FAILED: Inventory is empty.\"}");
        return;
    }
    
    // Extract the highest priority item (Lowest expiry days)
    Product p = extractMaxPriorityProduct(G_product_heap);
    
    // Try to find a PENDING request for this product
    LLNode* req_node = findRequest(G_request_list, p.product_name);
    
    if (req_node == NULL) {
        insertProduct(G_product_heap, p); // Put it back if no one wants it
        printf("{\"status\":\"warn\", \"message\":\"Top priority item (%s) has no pending buyers. System idle.\"}", p.product_name);
        return;
    }
    
    // Check if we can fulfill the request
    if (p.quantity >= req_node->data.quantity) {
        float remaining_qty = p.quantity - req_node->data.quantity;
        float price_per_kg = p.price_per_kg;
        float qty_requested = req_node->data.quantity;
        float total_cost = qty_requested * price_per_kg;
        
        updateRequestStatus(G_request_list, req_node->data.request_id, "ALLOCATED");
        
        if (remaining_qty > 0) {
            p.quantity = remaining_qty;
            insertProduct(G_product_heap, p); // Put remaining stock back
        }
        
        printf("{\"status\":\"success\", "
               "\"message\":\"Pending Request Fulfilled!\", "
               "\"bill\": {"
                   "\"req_id\":\"%s\", "
                   "\"buyer_id\":\"%s\", "
                   "\"product\":\"%s\", "
                   "\"farmer_id\":\"%s\", "
                   "\"qty_kg\":%.2f, "
                   "\"price_per_kg\":%.2f, "
                   "\"total_cost\":%.2f"
               "}}",
               req_node->data.request_id, req_node->data.buyer_id, p.product_name, p.farmer_id, qty_requested, price_per_kg, total_cost);
    } else {
        insertProduct(G_product_heap, p); 
        printf("{\"status\":\"error\", \"message\":\"Allocation FAILED: Not enough %s to meet pending request.\"}", p.product_name);
    }
}

// 6. ENGINE COMMAND: Get Insights (UPDATED: Sends farmer_id)
void engine_get_insights() {
    printf("{\"status\":\"success\", \"data\": {");
    printf("\"heap\": [");
    
    for (int i = 0; i < G_product_heap->size; i++) {
        printf("{\"name\":\"%s\", \"qty\":%.2f, \"expiry\":%d, \"price\":%.2f, \"farmer_id\":\"%s\"}",
               G_product_heap->array[i].product_name,
               G_product_heap->array[i].quantity,
               G_product_heap->array[i].freshness_score,
               G_product_heap->array[i].price_per_kg,
               G_product_heap->array[i].farmer_id); 
               
        if (i < G_product_heap->size - 1) printf(",");
    }
    
    printf("],");
    printf("\"requests\": [");
    LLNode* current = G_request_list->head;
    int node_count = 0;
    while(current) {
        if(node_count > 0) printf(",");
        printf("{\"id\":\"%s\", \"product\":\"%s\", \"qty\":%.2f, \"status\":\"%s\", \"buyer_id\":\"%s\"}",
               current->data.request_id,
               current->data.product_name,
               current->data.quantity,
               current->data.status,
               current->data.buyer_id);
        current = current->next;
        node_count++;
    }
    printf("]");
    printf("}}"); 
}

// 7. ENGINE COMMAND: Get Logistics
void engine_get_logistics(int start_node) {
    int dist[MAX_NODES];
    int parent[MAX_NODES];
    dijkstra(G_market_graph, start_node, dist, parent);
    printf("{\"status\":\"success\", \"paths\": [");
    int paths_found = 0;
    for (int i = 0; i < G_market_graph->num_nodes; i++) {
        if (i == start_node || dist[i] == INT_MAX) continue;
        if (paths_found > 0) printf(","); 
        printf("{\"node_name\":\"%s\", \"distance\":%d, \"path_str\":\"",
               G_market_graph->node_names[i], dist[i]);
        int curr = i;
        int path_count = 0;
        while(curr != -1) {
            if(path_count > 0) printf(" <- ");
            printf("%s", G_market_graph->node_names[curr]);
            curr = parent[curr];
            path_count++;
        }
        printf("\"}"); 
        paths_found++;
    }
    printf("]}"); 
}

// 8. ENGINE COMMAND: Cancel Request
void engine_cancel_request(const char* req_id) {
    LLNode* node = G_request_list->head;
    while(node != NULL && strcmp(node->data.request_id, req_id) != 0) {
        node = node->next;
    }
    if (node == NULL) {
        printf("{\"status\":\"error\", \"message\":\"Request ID not found.\"}");
        return;
    }
    if (strcmp(node->data.status, "PENDING") != 0) {
        printf("{\"status\":\"error\", \"message\":\"Cannot cancel a request that is already %s.\"}", node->data.status);
        return;
    }
    int result = cancelRequest(G_request_list, req_id); 
    if (result == 1) {
        printf("{\"status\":\"success\", \"message\":\"Request %s has been cancelled.\"}", req_id);
    } else {
        printf("{\"status\":\"error\", \"message\":\"Failed to cancel request %s.\"}", req_id);
    }
}


// --- The Main Engine Router ---
int main(int argc, char *argv[]) {
    initialize_engine_data();
    if (argc < 2) {
        printf("{\"status\":\"error\", \"message\":\"No command provided to engine.\"}");
        shutdown_engine_data();
        return 1;
    }
    char* command = argv[1];
    if (strcmp(command, "login") == 0 && argc == 4) {
        engine_login(argv[2], argv[3]);
    } 
    else if (strcmp(command, "register") == 0 && argc == 7) {
        engine_register_user(argv[2], argv[3], argv[4], argv[5], argv[6]);
    }
    else if (strcmp(command, "addProduct") == 0 && argc == 7) {
        engine_add_product(argv[2], (float)atof(argv[3]), (float)atof(argv[4]), atoi(argv[5]), argv[6]);
    }
    else if (strcmp(command, "addRequest") == 0 && argc == 5) {
        engine_add_request(argv[2], argv[3], (float)atof(argv[4]));
    }
    else if (strcmp(command, "runAllocation") == 0 && argc == 2) {
        engine_run_allocation();
    }
    else if (strcmp(command, "getInsights") == 0 && argc == 2) {
        engine_get_insights();
    }
    else if (strcmp(command, "getLogistics") == 0 && argc == 3) {
        engine_get_logistics(atoi(argv[2]));
    }
    else if (strcmp(command, "cancelRequest") == 0 && argc == 3) {
        engine_cancel_request(argv[2]);
    }
    else {
        printf("{\"status\":\"error\", \"message\":\"Unknown command or wrong arguments.\"}");
    }
    shutdown_engine_data();
    return 0; 
}