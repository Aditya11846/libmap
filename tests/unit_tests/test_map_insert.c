#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <map.h>

// Dummy key clone function
void* dummy_key_clone(void *key) {
    char *copy = malloc(strlen((char*)key) +1);
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
	(void)key;
    return NULL;  // Simulate failure
}

int main(void) {
    map_t *map;
    map_error_t result;

    // Create Map
    result = map_create(&map, dummy_key_clone, dummy_value_clone, dummy_hash,
                        dummy_stringify, dummy_compare, dummy_free_key, dummy_free_value);
    assert(result == MAP_OK && "Map creation failed");
    printf("Map created successfully!\n");

    // Insert Initial Elements
    char *keys[] = {"one", "two", "three", "four", "five"};
    int values[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        result = map_insert(map, keys[i], &values[i]);
        assert(result == MAP_OK && "Map insertion failed");
    }
    printf("Inserted 5 elements.\n");

    // Validate Size After Insertion
    int size;
    result = map_get_size(map, &size);
    assert(result == MAP_OK && "map_get_size failed");
    assert(size == 5 && "Map size incorrect after initial insertion");
    printf("Map size is correct: %d\n", size);

    // Force Resize by Adding More Elements (up to 50)
    for (int i = 6; i <= 50; i++) {
        char key[20];
        snprintf(key, sizeof(key), "key%d", i);
        result = map_insert(map, key, &i);
        assert(result == MAP_OK && "Map insertion failed during resizing");
    }
    printf("Inserted 50 elements to trigger resize.\n");

    // Validate Size After Resizing
    result = map_get_size(map, &size);
    assert(result == MAP_OK && "map_get_size failed");
    assert(size == 50 && "Map size incorrect after resizing");
    printf("Map resized and size is correct: %d\n", size);

	// Validate number of buckets after resizing
	int numbuckets;
	result = map_get_num_buckets(map, &numbuckets);
	assert(result == MAP_OK && "map_get_num_buckets failed");
	assert(numbuckets == 40 && "Number of buckets is correct after resizing");
	printf("Map resized and number of buckets is correct: %d\n", numbuckets);
    // Test Broken usr_key_clone
    map_t *broken_map;
    result = map_create(&broken_map, broken_key_clone, dummy_value_clone, dummy_hash,
                        dummy_stringify, dummy_compare, dummy_free_key, dummy_free_value);
    assert(result == MAP_OK && "Broken map creation failed");

    result = map_insert(broken_map, "broken_key", &values[0]);
    assert(result == MAP_ERR_NO_MEM && "Broken key clone should fail");
    printf("Correctly handled broken key clone.\n");

    map_destroy(&broken_map);

    // Cleanup
    map_destroy(&map);
    printf("Map destroyed successfully.\n");

    return MAP_OK;
}
