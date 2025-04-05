#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <map.h>

// Clone string key
void* dummy_key_clone(void *key) {
    char* copy = malloc(strlen((char*)key) +1);
    if (copy){
		strcpy(copy, (char*)key);
    }
    return copy;
}

// Clone integer value
void* dummy_value_clone(void *value) {
    int *new_value = malloc(sizeof(int));
    *new_value = *((int *)value);
    return new_value;
}

// Fake hash function to force chaining (returns same hash for all keys)
uint64_t fake_hash(void *key) {
    (void)key;
    return 42;  // Forces all keys into the same bucket
}

// Stringify key-value pair for display
char* dummy_stringify(void *key, void *value) {
    char *str = malloc(100 * sizeof(char));
    snprintf(str, 100, "(Key: %s, Value: %d)", (char *)key, *(int *)value);
    return str;
}

// Correct string compare function (Fixed!)
int32_t dummy_compare(void *key1, void *key2) {
    int result = strcmp((char *)key1, (char *)key2);
    if (result < 0) return -1;
    if (result > 0) return 1;
    return 0;
}


// Free key memory
void dummy_free_key(void *key) { free(key); }

// Free value memory
void dummy_free_value(void *value) { free(value); }

int main(void) {
    map_t *map;
    map_error_t result;

    // Create hashmap
    result = map_create(&map, dummy_key_clone, dummy_value_clone, fake_hash,
                        dummy_stringify, dummy_compare, dummy_free_key, dummy_free_value);
    assert(result == MAP_OK && "Map creation failed");
    printf("Map created successfully!\n");

    // Configure map with high load factor
    result = map_configure(map, 1000, 0.75, 2.0);
    assert(result == MAP_OK);
    printf("Map configured with high load factor (1000) for chaining.\n");

    // Insert elements (forcing collisions into the same bucket)
    int values[] = {10, 20, 30, 40, 50};
    char *keys[] = {"alpha", "beta", "gamma", "delta", "epsilon"};

    for (int i = 0; i < 5; i++) {
        result = map_insert(map, keys[i], &values[i]);
        assert(result == MAP_OK && "Map insertion failed");
        printf("Inserted: %s -> %d\n", keys[i], values[i]);
    }

    // Print current map state
    printf("\n Map after insertions:\n");
    map_print(map);

    // Retrieve and validate values
    for (int i = 0; i < 5; i++) {
        void *retrieved_value;
        printf("\n Looking for key: %s\n", keys[i]);
        result = map_get(map, keys[i], &retrieved_value);

        if (result != MAP_OK) {
            printf("map_get failed for key: %s (Error Code: %d)\n", keys[i], result);
        } else {
            printf("map_get success! %s -> %d\n", keys[i], *(int *)retrieved_value);
        }
        assert(result == MAP_OK && "map_get failed");
    }

    // Remove a key and check map state
    printf("\n Removing key: gamma\n");
    assert(map_remove(map, "gamma") == MAP_OK);
    map_print(map);

    // Verify size after removal
    int size;
    assert(map_get_size(map, &size) == MAP_OK);
    assert(size == 4);
    printf("\n Map size after removal: %d\n", size);

    // Cleanup
    assert(map_destroy(&map) == MAP_OK);
    printf("\n All assertions passed! Chaining test successful.\n");

    return MAP_OK;
}
