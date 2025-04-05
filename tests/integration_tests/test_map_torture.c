#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <map.h>

#define NUM_ENTRIES 10000

// Dummy key clone function
void* dummy_key_clone(void *key) {
    int *copy = malloc(sizeof(int));
    if (copy) *copy = *(int*)key;
    return copy;
}

// Dummy value clone function
void* dummy_value_clone(void *value) {
    int *new_value = malloc(sizeof(int));
    if (new_value) *new_value = *(int *)value;
    return new_value;
}

// Simple Modulus Hashing
uint64_t dummy_hash(void *key) {
    return (*(int *)key) % 1000003;  // Large prime for better distribution
}

// Dummy stringify function
char* dummy_stringify(void *key, void *value) {
    char *str = malloc(100 * sizeof(char));
    if (str) snprintf(str, 100, "(Key: %d, Value: %d)", *(int *)key, *(int *)value);
    return str;
}

// Dummy compare function
int32_t dummy_compare(void *key1, void *key2) {
    int a = *(int*)key1;
    int b = *(int*)key2;
    return (a > b) - (a < b);
}

// Dummy free functions
void dummy_free_key(void *key) { free(key); }
void dummy_free_value(void *value) { free(value); }

int main(void) {
    map_t *map;
    map_error_t result;

    // Create Map
    result = map_create(&map, dummy_key_clone, dummy_value_clone, dummy_hash,
                        dummy_stringify, dummy_compare, dummy_free_key, dummy_free_value);
    assert(result == MAP_OK && "Map creation failed");
    printf("Map created successfully!\n");

    srand(0);  // Seed the random number generator

    // Inserting one million key/value pairs
    printf("Inserting %d random key-value pairs...\n", NUM_ENTRIES);
    for (int i = 0; i < NUM_ENTRIES; i++) {
        int *key = malloc(sizeof(int));
        int *value = malloc(sizeof(int));

        assert(key && value && "Memory allocation failed");

        *key = rand();  // Generate a random key
        *value = (*key) * 2;

        result = map_insert(map, key, value);
        assert(result == MAP_OK && "Map Insertion Failed");

		// Free manually allocated key and value after insertion.
        free(key);
        free(value);
    }
    printf("Insertion completed.\n");

    // Iterating and modifying values
    printf("Modifying values...\n");
    map_iterator_t iter;
    map_error_t iter_result;
    void *key, *value;

    iter_result = map_iter_start(map, &iter);
    assert(iter_result == MAP_OK && "Iterator start failed!");

    while ((iter_result = map_iter_next(map, &iter, &key, &value)) == MAP_OK) {
        int *v = (int*)value;
        *v = (*(int*)key) * 3;  // Modify value
    }
    printf("Values modified successfully.\n");

    // Verifying modified values
    printf("Verifying modified values...\n");
    iter_result = map_iter_start(map, &iter);
    while ((iter_result = map_iter_next(map, &iter, &key, &value)) == MAP_OK) {
        assert(*(int*)value == *(int*)key * 3 && "Value mismatch!");
    }
    printf("Values verified successfully!\n");

    // Reseeding RNG to regenerate the same key sequence for retrievals
    srand(0);

    // Access the map a few times using `map_get()`
    printf("Testing random retrievals...\n");
    for (int i = 0; i < NUM_ENTRIES / 5; i++) {
        int search_key = rand();  // Re-generate the same key sequence

        int *retrieved_value;
        result = map_get(map, &search_key, (void**)&retrieved_value);
        assert(result == MAP_OK && *retrieved_value == (search_key) * 3 && "map_get() value mismatch!");
    }
    printf("Random retrievals completed successfully.\n");

    // Remove all elements
    printf("Removing all elements...\n");
    iter_result = map_iter_start(map, &iter);
    while ((iter_result = map_iter_next(map, &iter, &key, &value)) == MAP_OK) {
        result = map_remove(map, key);
        assert(result == MAP_OK && "map_remove() failed!");
    }
    printf("All elements removed successfully.\n");


    map_destroy(&map);
    printf("Map destroyed successfully\n");


}
