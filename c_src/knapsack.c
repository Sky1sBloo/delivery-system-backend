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

int compare_ids(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// Ensure all item quadruplets (id, weight, volume, value) are present and parse them
struct Item* parse_items(int argc, char* argv[], int num_items) {
    struct Item* items = malloc(num_items * sizeof(struct Item));
    if (!items) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    for (int i = 0; i < num_items; ++i) {
        int base = 4 + i * 4;
        items[i].id = atoi(argv[base]);
        items[i].weight = atoi(argv[base + 1]);
        items[i].volume = atoi(argv[base + 2]);
        items[i].value = atoi(argv[base + 3]);
    }
    return items;
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
void fill_dp_table(int capacity, int max_volume, int num_items, struct Item* items, struct Cell*** dp_ptr) {
    struct Cell** dp = calloc(capacity + 1, sizeof(struct Cell*));
    struct Cell** new_dp = calloc(capacity + 1, sizeof(struct Cell*));
    for (int curr_weight = 0; curr_weight <= capacity; ++curr_weight) {
        dp[curr_weight] = calloc(max_volume + 1, sizeof(struct Cell));
        new_dp[curr_weight] = calloc(max_volume + 1, sizeof(struct Cell));
    }

    for (int item_idx = 0; item_idx < num_items; ++item_idx) {
        int wt = items[item_idx].weight;
        int vol = items[item_idx].volume;
        int val = items[item_idx].value;
        int id = items[item_idx].id;

        for (int curr_weight = 0; curr_weight <= capacity; ++curr_weight) {
            for (int curr_volume = 0; curr_volume <= max_volume; ++curr_volume) {
                new_dp[curr_weight][curr_volume] = dp[curr_weight][curr_volume];
            }
        }

        for (int curr_weight = wt; curr_weight <= capacity; ++curr_weight) {
            for (int curr_volume = vol; curr_volume <= max_volume; ++curr_volume) {
                struct Cell skip = dp[curr_weight][curr_volume];  // Exclude current item
                struct Cell take = skip;

                struct Cell prev = dp[curr_weight - wt][curr_volume - vol];
                take.value = prev.value + val;
                take.count = prev.count + 1;
                take.id_sum = prev.id_sum + id;
                
                if (take.value > skip.value ||
                    (take.value == skip.value && (
                        take.count < skip.count ||
                        (take.count == skip.count && take.id_sum < skip.id_sum)))) {
                    new_dp[curr_weight][curr_volume] = take;
                }
            }
        }

        struct Cell** temp = dp;
        dp = new_dp;
        new_dp = temp;
    }

    *dp_ptr = dp;

    for (int curr_weight = 0; curr_weight <= capacity; ++curr_weight) {
        free(new_dp[curr_weight]);
    }
    free(new_dp);
}

/*
 * Backtrack to find selected item IDs
 * Start from bottom-right of the table and trace what items were chosen
 */
int* backtrack_items(int capacity, int max_volume, int num_items, struct Item* items, struct Cell** dp, int* out_count) {
    int* selected_ids = malloc(num_items * sizeof(int));
    int count = 0;
    int curr_weight = capacity, curr_volume = max_volume;

    for (int item_idx = num_items - 1; item_idx >= 0; --item_idx) {
        int wt = items[item_idx].weight;
        int vol = items[item_idx].volume;
        int val = items[item_idx].value;
        int id = items[item_idx].id;

        if (curr_weight >= wt && curr_volume >= vol) {
            struct Cell prev = dp[curr_weight - wt][curr_volume - vol];
            struct Cell curr = dp[curr_weight][curr_volume];
            if (curr.value == prev.value + val &&
                curr.count == prev.count + 1 &&
                curr.id_sum == prev.id_sum + id) {
                selected_ids[count++] = id;
                curr_weight -= wt;
                curr_volume -= vol;
            }
        }
    }

    *out_count = count;
    return selected_ids;
}

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

    struct Item* items = parse_items(argc, argv, num_items);

    struct Cell** dp;
    fill_dp_table(capacity, max_volume, num_items, items, &dp);

    int selected_count;
    int* selected_ids = backtrack_items(capacity, max_volume, num_items, items, dp, &selected_count);

    // Output selected item IDs
    if (selected_count == 0) {
        return 0;
    } else {
        // Sort in ascending order (to ensure consistent output with earlier IDs)
        qsort(selected_ids, selected_count, sizeof(int), compare_ids);
        for (int i = 0; i < selected_count; ++i) {
            printf("%d", selected_ids[i]);
            if (i != selected_count - 1) printf(" ");
        }
        printf("\n");
    }

    for (int curr_weight = 0; curr_weight <= capacity; ++curr_weight) {
        free(dp[curr_weight]);
    }
    free(dp);
    free(items);
    free(selected_ids);

    return 0;
}
