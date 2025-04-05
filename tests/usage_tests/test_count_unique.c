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

int count_unique(int *arr, size_t size) {
    map_t *map;
    map_error_t result;

    // Create Map
    result = map_create(&map, dummy_key_clone, dummy_value_clone, dummy_hash,
                        dummy_stringify, dummy_compare, dummy_free_key, dummy_free_value);
    assert(result == MAP_OK && "Map creation failed");

    for (size_t i = 0; i < size; i++) {
        int *key = malloc(sizeof(int));
        assert(key && "Memory allocation failed");
        *key = arr[i];

        // Check if the key already exists
        int *existing_freq = NULL;
        result = map_get(map, key, (void **)&existing_freq);

        if (result == MAP_OK) {
            // Key exists, increment its frequency
            (*existing_freq)++;
            free(key);  // Free the unused key
        } else {
            // Key doesn't exist, insert it with frequency = 1
            int *freq = malloc(sizeof(int));
            assert(freq && "Memory allocation failed");
            *freq = 1;

            result = map_insert(map, key, freq);
            if (result != MAP_OK) {
				free(key);
				free(freq);
				assert(result == MAP_OK && "Map insertion failed");
            }

            free(key);
            free(freq);
        }
    }

    // Get the number of unique elements
    int unique_count;
    map_get_size(map, &unique_count);

    // Destroy hashmap
    map_destroy(&map);

    return unique_count;
}


int main (void) {

	int arr[] = {1,2,2,3,3,4,5,5,6,7,8,8,8,9,10,11,11,11,12};
	size_t size = sizeof(arr)/sizeof(arr[0]); // bytes->num_elements

	printf("The given array is \n");
	printf("[");
	for (size_t i = 0; i < size; i++) {
	printf("%d",arr[i]);
	if (i < size - 1) {
		printf(",");

	}
	}
	printf("]\n");
	int unique_count = count_unique(arr, size);
	printf("The number of unique elemnts in the array is : %d\n",unique_count);


	return 0;

}
