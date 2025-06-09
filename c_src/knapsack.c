#include <stdio.h>
#include <stdlib.h>

struct Item {
    int id;
    int weight;
    int value;
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Error: Insufficient arguments.\n");
        return 1;
    }
    
    // Parse capacity and number of items
    int capacity = atoi(argv[1]);
    int num_items = atoi(argv[2]);
    
    // Validate if all item triplets are present
    if (argc < 3 + num_items * 3) {
        fprintf(stderr, "Error: Expected more item arguments.\n");
        return 1;
    }

    struct Item* items = (struct Item*)malloc(num_items * sizeof(struct Item));
    if (!items) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }
    
    // Parse item triplets from arguments
    for (int i = 0; i < num_items; ++i) {
        int base = 3 + i * 3;
        items[i].id = atoi(argv[base]);
        items[i].weight = atoi(argv[base + 1]);
        items[i].value = atoi(argv[base + 2]);
        
        // Input validation for negative weights/values
        if (items[i].weight < 0 || items[i].value < 0) {
            fprintf(stderr, "Error: Item %d has negative weight or value.\n", items[i].id);
            free(items);
            return 1;
        }
    }
    
    // max_value_at_capacity[w] holds the maximum value achievable with total weight w
    int* max_value_at_capacity = (int*)calloc(capacity + 1, sizeof(int));
    // last_item[w] store last item used for each capacity
    int* last_item = (int*)malloc((capacity + 1) * sizeof(int));
    for (int w = 0; w <= capacity; ++w) last_item[w] = -1;

    for (int i = 0; i < num_items; ++i) {
        // Traverse backwards to avoid overwriting needed values
        for (int w = capacity; w >= items[i].weight; --w) {
            int new_val = max_value_at_capacity[w - items[i].weight] + items[i].value;
            if (new_val > max_value_at_capacity[w]) {
                max_value_at_capacity[w] = new_val;
                last_item[w] = i;
            }
        }
    }

    // Backtrack to find selected items
    int* selected_ids = (int*)malloc(num_items * sizeof(int));
    int count = 0, w = capacity;
    while (w > 0 && last_item[w] != -1) {
        int idx = last_item[w];
        selected_ids[count++] = items[idx].id;
        w -= items[idx].weight;
    }

    // Output selected item IDs in order
    for (int i = count - 1; i >= 0; --i) {
        printf("%d", selected_ids[i]);
        if (i != 0) printf(" ");
    }
    printf("\n");

    free(max_value_at_capacity);
    free(last_item);
    free(items);
    free(selected_ids);
    return 0;
}