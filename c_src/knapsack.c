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

    // Parse knapsack capacity and number of items
    int capacity = atoi(argv[1]);
    int num_items = atoi(argv[2]);

    // Ensure all item triplets (id, weight, value) are present
    if (argc < 3 + num_items * 3) {
        fprintf(stderr, "Error: Expected more item arguments.\n");
        return 1;
    }

    // Allocate memory for items
    struct Item* items = (struct Item*)malloc(num_items * sizeof(struct Item));
    if (!items) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    // Read item details from command-line arguments
    for (int i = 0; i < num_items; ++i) {
        int base = 3 + i * 3;
        items[i].id = atoi(argv[base]);
        items[i].weight = atoi(argv[base + 1]);
        items[i].value = atoi(argv[base + 2]);
    }

    // Allocate dynamic programming table
    int** value_table = (int**)malloc((num_items + 1) * sizeof(int*));
    if (!value_table) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(items);
        return 1;
    }

    for (int i = 0; i <= num_items; ++i) {
        value_table[i] = (int*)calloc(capacity + 1, sizeof(int));
        if (!value_table[i]) {
            fprintf(stderr, "Memory allocation failed.\n");
            for (int j = 0; j < i; ++j) free(value_table[j]);
            free(value_table);
            free(items);
            return 1;
        }
    }

    /*
     * value_table[i][w] stores the maximum value achievable using
     * the first i items with a weight limit of w.
     *
     * For each item, we check:
     * - If we can include it (item.weight <= current capacity w),
     *     then: max(included value, excluded value)
     * - If not, we copy the value from the row above (exclude)
     */
    for (int i = 1; i <= num_items; ++i) {
        for (int w = 0; w <= capacity; ++w) {
            if (items[i-1].weight <= w) {
                int included = value_table[i-1][w - items[i-1].weight] + items[i-1].value;
                int excluded = value_table[i-1][w];
                value_table[i][w] = (included > excluded) ? included : excluded;
            } else {
                value_table[i][w] = value_table[i-1][w];
            }
        }
    }

    // Backtrack to find selected items
    int* selected_ids = (int*)malloc(num_items * sizeof(int));
    if (!selected_ids) {
        fprintf(stderr, "Memory allocation failed.\n");
        for (int i = 0; i <= num_items; ++i) free(value_table[i]);
        free(value_table);
        free(items);
        return 1;
    }

    int count = 0;
    int w = capacity;
    for (int i = num_items; i > 0; --i) {
        if (w >= items[i-1].weight &&
            value_table[i][w] != value_table[i-1][w]) {
            selected_ids[count++] = items[i-1].id;
            w -= items[i-1].weight;
        }
    }

    // Output selected item IDs
    if (count == 0) {
        printf("No items selected\n");
    } else {
        for (int i = count - 1; i >= 0; --i) {
            printf("%d", selected_ids[i]);
            if (i != 0) printf(" ");
        }
        printf("\n");
    }

    // Free all allocated memory
    for (int i = 0; i <= num_items; ++i) free(value_table[i]);
    free(value_table);
    free(items);
    free(selected_ids);

    return 0;
}
