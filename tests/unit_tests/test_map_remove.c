#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <map.h>

// Dummy key clone function
void* dummy_key_clone(void *key) {
    char *copy = malloc(strlen((char*)key) + 1);
    if (copy) {
		strcpy(copy, (char*)key);
    }
    return copy;
}

// Dummy value clone function
void* dummy_value_clone(void *value) {
    int *new_value = malloc(sizeof(int));
    *new_value = *((int *)value);
    return new_value;
}

// Dummy hash function (djb2)
uint64_t dummy_hash(void *key) {
    char *str = (char *)key;
    uint64_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// Dummy stringify function
char* dummy_stringify(void *key, void *value) {
    char *str = malloc(100 * sizeof(char));
    snprintf(str, 100, "(Key: %s, Value: %d)", (char *)key, *(int *)value);
    return str;
}

// Dummy compare function
int32_t dummy_compare(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2);
}

// Dummy free functions
void dummy_free_key(void *key) {
    free(key);
}

void dummy_free_value(void *value) {
    free(value);
}

// Broken key clone function for error testing
void* broken_key_clone(void *key) {
	(void) key;
    return NULL;  // Simulate failure
}


int main(void) {
    map_t *map = NULL;
	map_error_t result;
    // Create the map

    result = map_create(&map, dummy_key_clone, dummy_value_clone, dummy_hash,
                        dummy_stringify, dummy_compare, dummy_free_key, dummy_free_value);
    assert(result == MAP_OK && "Map creation failed");
    printf("Map created successfully!\n");

    // Insert elements
    int keys[] = {1, 2, 3, 4, 5};
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        assert(map_insert(map, &keys[i], &values[i]) == MAP_OK);
    }

    // Check size
    int size;
    assert(map_get_size(map, &size) == MAP_OK);
    assert(size == 5);

    // Remove an element
    assert(map_remove(map, &keys[2]) == MAP_OK); // Remove key=3
    assert(map_get_size(map, &size) == MAP_OK);
    assert(size == 4);

    // Verify element is removed
    void *value;
    assert(map_get(map, &keys[2], &value) == MAP_ERR_NOT_FOUND);

    // Try to remove a non-existing element
    int non_existent_key = 99;
    assert(map_remove(map, &non_existent_key) == MAP_ERR_NOT_FOUND);

    // Ensure resizing occurs when elements are removed
    map_configure(map, 0.8, 0.3, 2.0); // Set min load factor to trigger shrinking
    assert(map_remove(map, &keys[0]) == MAP_OK);
    assert(map_remove(map, &keys[1]) == MAP_OK);
    assert(map_remove(map, &keys[3]) == MAP_OK);
    assert(map_remove(map, &keys[4]) == MAP_OK);

    assert(map_get_size(map, &size) == MAP_OK);
    assert(size == 0);

    // Destroy the map
    assert(map_destroy(&map) == MAP_OK);

    printf("All map_remove tests passed successfully!\n");
    return MAP_OK;
}
