#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <map.h>

// Clone string key
void* dummy_key_clone(void *key) {
    char *copy = malloc(strlen((char*)key) + 1);
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

// Simple hash function (djb2)
uint64_t dummy_hash(void *key) {
    char *str = (char *)key;
    uint64_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// Stringify key-value pair for display
char* dummy_stringify(void *key, void *value) {
    char *str = malloc(100 * sizeof(char));
    snprintf(str, 100, "(Key: %s, Value: %d)", (char *)key, *(int *)value);
    return str;
}

// Compare two string keys
int32_t dummy_compare(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2);
}

// Broken compare function for error testing
int32_t broken_compare(void *key1, void *key2) {
	(void)key1;
	(void)key2;
    return 42;
}

// Free key memory
void dummy_free_key(void *key) {
    free(key);
}

// Free value memory
void dummy_free_value(void *value) {
    free(value);
}

int main(void) {
    map_t *map;
    map_error_t result;

    // Create hashmap with dummy functions
    result = map_create(&map, dummy_key_clone, dummy_value_clone, dummy_hash,
                        dummy_stringify, dummy_compare, dummy_free_key, dummy_free_value);
    assert(result == MAP_OK && "Map creation failed");
    printf("Map created successfully\n");

    // Insert initial key-value pairs
    char *keys[] = {"alpha", "beta", "gamma"};
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; i++) {
        result = map_insert(map, keys[i], &values[i]);
        assert(result == MAP_OK && "Map insertion failed");
    }
    printf("Inserted 3 elements\n");

    // Retrieve existing keys and validate values
    for (int i = 0; i < 3; i++) {
        void *retrieved_value;
        result = map_get(map, keys[i], &retrieved_value);
        assert(result == MAP_OK && "Failed to get existing key");
        printf("Retrieved: %s -> %d\n", keys[i], *(int *)retrieved_value);
    }

    // Attempt to retrieve a non-existent key
    void *retrieved_value;
    result = map_get(map, "delta", &retrieved_value);
    assert(result == MAP_ERR_NOT_FOUND && "Non-existent key should return MAP_ERR_NOT_FOUND");
    printf("Correctly handled non-existent key\n");

    // Create a new map with a broken compare function
    map_t *broken_map;
    result = map_create(&broken_map, dummy_key_clone, dummy_value_clone, dummy_hash,
                        dummy_stringify, broken_compare, dummy_free_key, dummy_free_value);
    assert(result == MAP_OK && "Broken map creation failed");

    // Insert into broken map
    result = map_insert(broken_map, "key", &values[0]);
    assert(result == MAP_OK && "Failed to insert into broken map");

    // Attempt to retrieve with broken compare
    result = map_get(broken_map, "key", &retrieved_value);
    assert(result == MAP_ERR_UNKNOWN && "Broken compare should return MAP_ERR_UNKNOWN");
    printf("Correctly handled broken compare function\n");

    // Clean up maps
    map_destroy(&map);
    map_destroy(&broken_map);
    printf("All tests passed\n");

    return MAP_OK;
}
