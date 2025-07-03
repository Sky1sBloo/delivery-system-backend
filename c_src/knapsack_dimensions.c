#include <stdio.h>
#include <stdlib.h>

struct item {
    int id;
    int weight;
    int volume;
    int value;
};

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "error: insufficient arguments.\n");
        return 1;
    }

    // parse knapsack capacity, max volume, and number of items
    int capacity = atoi(argv[1]);
    int max_volume = atoi(argv[2]);
    int num_items = atoi(argv[3]);

    // ensure all item quadruplets (id, weight, volume, value) are present
    if (argc < 4 + num_items * 4) {
        fprintf(stderr, "error: expected more item arguments.\n");
        return 1;
    }

    // allocate memory for items
    struct item* items = (struct item*)malloc(num_items * sizeof(struct item));
    if (!items) {
        fprintf(stderr, "memory allocation failed.\n");
        return 1;
    }

    // read item details from command-line arguments
    for (int item_idx = 0; item_idx < num_items; ++item_idx) {
        int base = 4 + item_idx * 4;
        items[item_idx].id = atoi(argv[base]);
        items[item_idx].weight = atoi(argv[base + 1]);
        items[item_idx].volume = atoi(argv[base + 2]);
        items[item_idx].value = atoi(argv[base + 3]);
    }

    /*
     * allocate 3d dynamic programming table:
     * dp[i][w][v] represents the maximum value achievable using the first i items
     * with a weight limit of w and volume limit of v.
     */
    int*** dp = malloc((num_items + 1) * sizeof(int**));
    int*** keep = malloc((num_items + 1) * sizeof(int**));
    for (int i = 0; i <= num_items; ++i) {
        dp[i] = malloc((capacity + 1) * sizeof(int*));
        keep[i] = malloc((capacity + 1) * sizeof(int*));
        for (int w = 0; w <= capacity; ++w) {
            dp[i][w] = calloc(max_volume + 1, sizeof(int));
            keep[i][w] = calloc(max_volume + 1, sizeof(int));
        }
    }

    /*
     * fill the dynamic programming table.
     * for each item, we decide whether to include it or not based on:
     * - if weight and volume constraints allow it
     * - taking the maximum of:
     *     a) excluding the item (value remains from previous row)
     *     b) including the item (value = previous value at reduced weight/volume + item value)
     */
    for (int item_idx = 1; item_idx <= num_items; ++item_idx) {
        for (int curr_weight = 0; curr_weight <= capacity; ++curr_weight) {
            for (int curr_volume = 0; curr_volume <= max_volume; ++curr_volume) {
                int value_if_skipped = dp[item_idx-1][curr_weight][curr_volume];  // exclude current item
                int value_if_taken = 0;

                if (curr_weight >= items[item_idx-1].weight && curr_volume >= items[item_idx-1].volume) {
                    value_if_taken = dp[item_idx-1][curr_weight - items[item_idx-1].weight][curr_volume - items[item_idx-1].volume] + items[item_idx-1].value;
                }

                if (value_if_taken >= value_if_skipped) {
                    dp[item_idx][curr_weight][curr_volume] = value_if_taken;
                    keep[item_idx][curr_weight][curr_volume] = 1;  // mark item as taken
                } else {
                    dp[item_idx][curr_weight][curr_volume] = value_if_skipped;
                }
            }
        }
    }

    /*
     * backtrack to find selected item ids
     * start from bottom-right of the table and trace what items were chosen
     */
    int* selected_ids = malloc(num_items * sizeof(int));
    int count = 0;
    int curr_weight = capacity;
    int curr_volume = max_volume;

    for (int item_idx = num_items; item_idx > 0; --item_idx) {
        if (keep[item_idx][curr_weight][curr_volume]) {
            selected_ids[count++] = items[item_idx-1].id;
            curr_weight -= items[item_idx-1].weight;
            curr_volume -= items[item_idx-1].volume;
        }
    }

    // output selected item ids
    if (count == 0) {
        return 0;
    } else {
        for (int i = count - 1; i >= 0; --i) {
            printf("%d", selected_ids[i]);
            if (i != 0) printf(" ");
        }
        printf("\n");
    }

    // free all allocated memory
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
