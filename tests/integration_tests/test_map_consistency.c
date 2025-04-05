#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <map.h>

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

// Main test function
int main(void) {
    map_t *map;
    map_error_t result;

    // Create Map
    result = map_create(&map, dummy_key_clone, dummy_value_clone, dummy_hash,
                        dummy_stringify, dummy_compare, dummy_free_key, dummy_free_value);
    assert(result == MAP_OK && "Map creation failed");
    printf("Map created successfully!\n");

    // Insert 1000 elements
    for (int i = 0; i < 1000; i++) {
        int *key = malloc(sizeof(int));
        int *value = malloc(sizeof(int));

        assert(key && value && "Memory allocation failed!");

        *key = i;
        *value = i * i;
        result = map_insert(map, key, value);

        if (result != MAP_OK) {
			free(key);
			free(value);
			assert(result == MAP_OK && "Map insertion failed!");
        }

        free(key);
        free(value);

    }

    printf("All elements inserted successfully!\n");
    printf("Entries: %d, Buckets: %d\n", map->num_entries, map->num_buckets);

    // Retrieve all inserted elements
    for (int i = 0; i < 1000; i++) {
        int key = i;
        void *retrieved_value;
        result = map_get(map, &key, &retrieved_value);
        assert(result == MAP_OK && "Failed to get key");
        /* printf("Retrieved: %d -> %d\n", key, *(int*)retrieved_value); */
    }

	// Iterating over the map
	map_iterator_t iter;
	map_error_t iter_result;
	int count = 0;

	iter_result = map_iter_start(map, &iter);
	assert(iter_result == MAP_OK && "Iterator start failed!");

	void*key;
	void*value;

	while((iter_result = map_iter_next(map, &iter, &key, &value)) == MAP_OK) {
		count++;
		/* printf("Iterating: Key = %d, Value = %d\n", *(int*)key, *(int*)value); */
	}
	printf("Final iteration count: %d\n", count);
	assert(count == 1000 && "Iteration count failed");
	printf("Iteration successful, count: %d\n",count);

	// Removal
	for (int i = 250; i < 750; i++) {
		int key = i;
		result = map_remove(map, &key);
		assert(result == MAP_OK && "Failed to remove key");
	}
	printf("Elemnts in the range 250-750 removed successfully!\n");
	printf("Entries: %d, Buckets: %d\n", map->num_entries, map->num_buckets);

	// Iterating again
	count = 0;
	iter_result = map_iter_start(map, &iter);
	assert(iter_result == MAP_OK && "Iterator start failed!");
	while ((iter_result = map_iter_next(map, &iter, &key, &value)) == MAP_OK) {
		int k = *(int*)key;
		int v = *(int*)value;
		count++;
		assert(v == k * k && "Value mismatch (not square)");
		/* printf("Iterating after removal: key = %d, Value = %d\n",k,v); */
	}

	printf("Final iteration count after removal: %d\n", count);
	assert(count == 500 && "Iteration count after removal failed!");
	printf("Final iteration successful, remaining elements: %d\n", count);

	map_destroy(&map);
	printf("Map destroyed successfully!\n");

	return MAP_OK;

}
