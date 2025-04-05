#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

// Dummy compare function
int32_t dummy_compare(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2);
}

// Dummy stringify function
char* dummy_stringify(void *key, void *value) {
    char *str = malloc(100 * sizeof(char));
    snprintf(str, 100, "(Key: %s, Value: %d)", (char *)key, *(int *)value);
    return str;
}

// Dummy free functions
void dummy_free_key(void *key) {
    free(key);
}

void dummy_free_value(void *value) {
    free(value);
}

int main(void){
	map_t *map;
	map_error_t result;

	// Creating the map
	result = map_create(&map, dummy_key_clone, dummy_value_clone, dummy_hash,
						dummy_stringify, dummy_compare, dummy_free_key, dummy_free_value);

	assert(result == MAP_OK && "Map creation failed!\n");
	printf("Map created successfully!\n");

	// Destroying the map
	result = map_destroy(&map);
	printf("Map destroyed successfully!\n");

	assert(map == NULL && "Map destruction failed!\n");
	return MAP_OK;

}
