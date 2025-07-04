#include <stdio.h>
#include <stdlib.h>

#define HUGE_RATIO 1e9

typedef struct {
    int id;
    int weight;
    int volume;
    int value;
    double ratio;
} Item;

int compareItems(const void *a, const void *b) {
    Item *itemA = (Item*)a;
    Item *itemB = (Item*)b;

    if (itemA->ratio > itemB->ratio) return -1;
    if (itemA->ratio < itemB->ratio) return 1;

    // Tie-breaker 1: higher value
    if (itemA->value > itemB->value) return -1;
    if (itemA->value < itemB->value) return 1;

    // Tie-breaker 2: lower weight 
    if (itemA->weight < itemB->weight) return -1;
    if (itemA->weight > itemB->weight) return 1;

    // Tie-breaker 3: lower ID
    return itemA->id - itemB->id;
}

// Parse arguments and outputs it to [items] param
int parseArguments(int argc, char *argv[], int *capacity, int *max_volume, int *num_items, Item **items) {
    if (argc < 4) {
        fprintf(stderr, "Error: Not enough arguments. Usage: <capacity> <max_volume> <num_items> <id1> <w1> <v1> <val1> ...\n");
        return 101;      
    }

    *capacity = atoi(argv[1]);
    *max_volume = atoi(argv[2]);
    *num_items = atoi(argv[3]);

    int expected_args = 4 + (*num_items * 4);
    if (argc != expected_args) {
        fprintf(stderr, "Error: Expected %d arguments, got %d\n", expected_args, argc);
        return 102;     }

    *items = (Item*)malloc(*num_items * sizeof(Item));
    if (!*items) {
        fprintf(stderr, "Error: Memory allocation failed for items.\n");
        return 103;     }

        // item fields and compute ratio = value / (weight + volume)
        for (int i = 0; i < *num_items; i++) {
            int base_idx = 4 + i * 4;
            
            int id     = atoi(argv[base_idx]);
            int weight = atoi(argv[base_idx + 1]);
            int volume = atoi(argv[base_idx + 2]);
            int value  = atoi(argv[base_idx + 3]);

            int denom = weight + volume;
            double ratio = (denom > 0) ? (double)value / denom : HUGE_RATIO; // Prefer items with 0 weight+volume

            (*items)[i].id     = id;
            (*items)[i].weight = weight;
            (*items)[i].volume = volume;
            (*items)[i].value  = value;
            (*items)[i].ratio  = ratio;
        }

    return 0;  }

void sortItemsByRatio(Item *items, int num_items) {
    qsort(items, num_items, sizeof(Item), compareItems);
}

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

void printSelectedIds(int *selected_ids, int count) {
    for (int i = 0; i < count; i++) {
        printf("%d", selected_ids[i]);
        if (i < count - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

void cleanup(Item *items, int *selected_ids) {
    free(items);
    free(selected_ids);
}

int main(int argc, char *argv[]) {
    int capacity, max_volume, num_items;
    Item *items;

    int parse_result = parseArguments(argc, argv, &capacity, &max_volume, &num_items, &items);
    if (parse_result != 0) {
        return parse_result;  // Return specific error code
    }

    int *selected_ids = (int*)malloc(num_items * sizeof(int));
    if (!selected_ids) {
        fprintf(stderr, "Error: Memory allocation failed for selected_ids.\n");
        free(items);
        return 104;  // Memory allocation failure for selected_ids
    }

    sortItemsByRatio(items, num_items);

    int selected_count = greedyKnapsack(items, num_items, capacity, max_volume, selected_ids);

    printSelectedIds(selected_ids, selected_count);

    cleanup(items, selected_ids);

    return 0;
}
