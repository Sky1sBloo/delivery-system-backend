#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Item {
    int id;
    int weight;
    int volume;
    int value;
};

// Struct to store value, number of items used, and total ID sum for tiebreaking
struct Cell {
    int value;
    int count;
    int id_sum;
};

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Error: Insufficient arguments.\n");
        return 1;
    }

    // Parse knapsack capacity, max volume, and number of items
    int capacity = atoi(argv[1]);
    int max_volume = atoi(argv[2]);
    int num_items = atoi(argv[3]);

    // Ensure all item quadruplets (id, weight, volume, value) are present
    if (argc < 4 + num_items * 4) {
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
    for (int item_idx = 0; item_idx < num_items; ++item_idx) {
        int base = 4 + item_idx * 4;
        items[item_idx].id = atoi(argv[base]);
        items[item_idx].weight = atoi(argv[base + 1]);
        items[item_idx].volume = atoi(argv[base + 2]);
        items[item_idx].value = atoi(argv[base + 3]);
    }

    /*
     * Allocate 3D dynamic programming table:
     * dp[i][w][v] represents the maximum value achievable using the first i items
     * with a weight limit of w and volume limit of v.
     */
    struct Cell*** dp = malloc((num_items + 1) * sizeof(struct Cell**));
    int*** keep = malloc((num_items + 1) * sizeof(int**));
    for (int i = 0; i <= num_items; ++i) {
        dp[i] = malloc((capacity + 1) * sizeof(struct Cell*));
        keep[i] = malloc((capacity + 1) * sizeof(int*));
        for (int w = 0; w <= capacity; ++w) {
            dp[i][w] = calloc(max_volume + 1, sizeof(struct Cell));
            keep[i][w] = calloc(max_volume + 1, sizeof(int));
        }
    }

    /*
     * Fill the dynamic programming table.
     * For each item, we decide whether to include it or not based on:
     * - If weight and volume constraints allow it
     * - Taking the maximum of:
     *     a) Excluding the item (value remains from previous row)
     *     b) Including the item (value = previous value at reduced weight/volume + item value)
     *       Tiebreaker rules:
     *         - Prefer fewer items (lower count)
     *         - If tied, prefer lower sum of item IDs (earlier items)
     */
    for (int item_idx = 1; item_idx <= num_items; ++item_idx) {
        for (int curr_weight = 0; curr_weight <= capacity; ++curr_weight) {
            for (int curr_volume = 0; curr_volume <= max_volume; ++curr_volume) {
                struct Cell skip = dp[item_idx - 1][curr_weight][curr_volume];  // Exclude current item
                struct Cell take = skip;

                if (curr_weight >= items[item_idx - 1].weight &&
                    curr_volume >= items[item_idx - 1].volume) {

                    struct Cell prev = dp[item_idx - 1][curr_weight - items[item_idx - 1].weight][curr_volume - items[item_idx - 1].volume];

                    take.value = prev.value + items[item_idx - 1].value;
                    take.count = prev.count + 1;
                    take.id_sum = prev.id_sum + items[item_idx - 1].id;
                }

                // Choose better of skip vs take
                if (take.value > skip.value ||
                    (take.value == skip.value && (
                        take.count < skip.count ||
                        (take.count == skip.count && take.id_sum < skip.id_sum)
                    ))) {
                    dp[item_idx][curr_weight][curr_volume] = take;
                    keep[item_idx][curr_weight][curr_volume] = 1;  // Mark item as taken
                } else {
                    dp[item_idx][curr_weight][curr_volume] = skip;
                    keep[item_idx][curr_weight][curr_volume] = 0;  // Skip item
                }
            }
        }
    }

    /*
     * Backtrack to find selected item IDs
     * Start from bottom-right of the table and trace what items were chosen
     */
    int* selected_ids = malloc(num_items * sizeof(int));
    int count = 0;
    int curr_weight = capacity;
    int curr_volume = max_volume;

    for (int item_idx = num_items; item_idx > 0; --item_idx) {
        if (keep[item_idx][curr_weight][curr_volume]) {
            selected_ids[count++] = items[item_idx - 1].id;
            curr_weight -= items[item_idx - 1].weight;
            curr_volume -= items[item_idx - 1].volume;
        }
    }

    // Output selected item IDs
    if (count == 0) {
        return 0;
    } else {
        // Sort in ascending order (to ensure consistent output with earlier IDs)
        for (int i = 0; i < count - 1; ++i) {
            for (int j = i + 1; j < count; ++j) {
                if (selected_ids[i] > selected_ids[j]) {
                    int tmp = selected_ids[i];
                    selected_ids[i] = selected_ids[j];
                    selected_ids[j] = tmp;
                }
            }
        }
        for (int i = 0; i < count; ++i) {
            printf("%d", selected_ids[i]);
            if (i != count - 1) printf(" ");
        }
        printf("\n");
    }

    // Free all allocated memory
    for (int i = 0; i <= num_items; ++i) {
        for (int w = 0; w <= capacity; ++w) {
            free(dp[i][w]);
            free(keep[i][w]);
        }
        free(dp[i]);
        free(keep[i]);
    }
    free(dp);
    free(keep);
    free(items);
    free(selected_ids);

    return 0;
}
