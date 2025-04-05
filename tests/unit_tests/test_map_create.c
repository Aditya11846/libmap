#include <stdio.h>
#include <map.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Dummy key clone function for integers
void* dummy_int_key_clone(void *key) {
    if (!key) return NULL;
    int *new_key = malloc(sizeof(int));
    *new_key = *(int *)key;
    return new_key;
}

// Dummy value clone function for integers
void* dummy_int_value_clone(void *value) {
    if (!value) return NULL;
    int *new_value = malloc(sizeof(int));
    *new_value = *(int *)value;
    return new_value;
}

// Integer-based hash function
uint64_t dummy_user_hash(void *key) {
    int *int_key = (int *)key;
    return (uint64_t)(*int_key);
}

// Stringify function for printing keys and values
char* dummy_user_stringify(void *key, void *value) {
    int *k = (int *)key;
    int *v = (int *)value;
    char *str = malloc(100 * sizeof(char));
    if (str != NULL) {
        snprintf(str, 100, "(Key: %d, Value: %d)", *k, *v);
    }
    return str;
}

// Integer comparison
int32_t dummy_user_compare(void *key1, void *key2) {
    int *k1 = (int *)key1;
    int *k2 = (int *)key2;
    return (*k1 - *k2);
}

// Dummy free functions
void dummy_user_free_key(void *key){
	free(key);
}

void dummy_user_free_value(void *value){
	free(value);
}

int main(void) {
    map_t *map;
    printf("Creating Map...\n");

    // Create map
    map_error_t result = map_create(&map, dummy_int_key_clone, dummy_int_value_clone, dummy_user_hash,
                                    dummy_user_stringify, dummy_user_compare, dummy_user_free_key, dummy_user_free_value);
    assert(result == MAP_OK && "Map creation failed");
    printf("Map created successfully!\n");

    // Test invalid argument (passing NULL)
    result = map_create(NULL, dummy_int_key_clone, dummy_int_value_clone, dummy_user_hash,
                        dummy_user_stringify, dummy_user_compare, dummy_user_free_key, dummy_user_free_value);
    assert(result == MAP_ERR_INVALID_ARG && "NULL pointer should return MAP_ERR_INVALID_ARG");

    // Clean up
    map_destroy(&map);

    return MAP_OK;
}

