#include <stdio.h>
#include <stdlib.h>

// Structure to store each item's attributes and value-to-weight ratio
typedef struct {
    int id;
    int weight;
    int volume;
    int value;
    double ratio;
} Item;

// Parses command-line arguments and populates the items array
int parseArguments(int argc, char *argv[], int *capacity, int *max_volume, int *num_items, Item **items) {
    if (argc < 4) {
        fprintf(stderr, "Error: Not enough arguments. Usage: <capacity> <max_volume> <num_items> <id1> <w1> <v1> <val1> ...\n");
        return 101;  // Error code for insufficient base arguments
    }

    *capacity = atoi(argv[1]);
    *max_volume = atoi(argv[2]);
    *num_items = atoi(argv[3]);

    int expected_args = 4 + (*num_items * 4);
    if (argc != expected_args) {
        fprintf(stderr, "Error: Expected %d arguments, got %d\n", expected_args, argc);
        return 102;  // Error code for incorrect total number of arguments
    }

    *items = (Item*)malloc(*num_items * sizeof(Item));
    if (!*items) {
        fprintf(stderr, "Error: Memory allocation failed for items.\n");
        return 103;  // Memory allocation failure
    }

    // Parse item fields and compute ratio
    for (int i = 0; i < *num_items; i++) {
        int base_idx = 4 + i * 4;
        (*items)[i].id = atoi(argv[base_idx]);
        (*items)[i].weight = atoi(argv[base_idx + 1]);
        (*items)[i].volume = atoi(argv[base_idx + 2]);
        (*items)[i].value = atoi(argv[base_idx + 3]);

        // If weight is zero, assign a very high ratio (prefer it)
        if ((*items)[i].weight > 0) {
            (*items)[i].ratio = (double)(*items)[i].value / (*items)[i].weight;
        } else {
            (*items)[i].ratio = 1e9;
        }
    }

    return 0;  // Success
}

// Sorts the items in descending order based on value-to-weight ratio
void sortItemsByRatio(Item *items, int num_items) {
    for (int i = 0; i < num_items - 1; i++) {
        for (int j = 0; j < num_items - i - 1; j++) {
            if (items[j].ratio < items[j + 1].ratio) {
                Item temp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = temp;
            }
        }
    }
}

// Greedy selection based on sorted ratio, respecting both weight and volume limits
int greedyKnapsack(Item *items, int num_items, int capacity, int max_volume, int *selected_ids) {
    int total_weight = 0;
    int total_volume = 0;
    int selected_count = 0;

    for (int i = 0; i < num_items; i++) {
        if (total_weight + items[i].weight <= capacity &&
            total_volume + items[i].volume <= max_volume) {
            
            selected_ids[selected_count] = items[i].id;
            selected_count++;
            total_weight += items[i].weight;
            total_volume += items[i].volume;
        }
    }

    return selected_count;
}

// Outputs selected item IDs space-separated
void printSelectedIds(int *selected_ids, int count) {
    for (int i = 0; i < count; i++) {
        printf("%d", selected_ids[i]);
        if (i < count - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

// Frees dynamically allocated memory
void cleanup(Item *items, int *selected_ids) {
    free(items);
    free(selected_ids);
}

int main(int argc, char *argv[]) {
    int capacity, max_volume, num_items;
    Item *items;

    // Parse command-line arguments and validate input
    int parse_result = parseArguments(argc, argv, &capacity, &max_volume, &num_items, &items);
    if (parse_result != 0) {
        return parse_result;  // Return specific error code
    }

    // Allocate memory for selected item IDs
    int *selected_ids = (int*)malloc(num_items * sizeof(int));
    if (!selected_ids) {
        fprintf(stderr, "Error: Memory allocation failed for selected_ids.\n");
        free(items);
        return 104;  // Memory allocation failure for selected_ids
    }

    // Sort by value-to-weight ratio
    sortItemsByRatio(items, num_items);

    // Greedy selection of items
    int selected_count = greedyKnapsack(items, num_items, capacity, max_volume, selected_ids);

    // Output selected item IDs
    printSelectedIds(selected_ids, selected_count);

    // Cleanup
    cleanup(items, selected_ids);

    return 0;
}
