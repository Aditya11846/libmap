#pragma once
#include <map_internal.h>
#include <stdint.h>
#include <stdio.h>
#include <types.h>

#define NUM_INITIAL_BUCKETS 10

// Error reporting function.
void map_log_error(map_error_t err, const char *message);

// Create a new map instance, registering all the user functions and
// setting default values for factors.
map_error_t map_create(map_t **map, void *(*usr_key_clone)(void *key),
                       void *(*usr_value_clone)(void *value),
                       uint64_t (*usr_hash)(void *key),
                       char *(*usr_stringify)(void *key, void *data),
                       int32_t (*usr_compare)(void *key1, void *key2),
                       void (*usr_free_key)(void *key),
                       void (*usr_free_value)(void *value));

// Return the number of elements in the hashmap.
map_error_t map_get_size(map_t *map, int *num_elements);

// Return the number of buckets in the hashmap.
map_error_t map_get_num_buckets(map_t *map, int *num_buckets);

// Allow the user to configure factors. Ensure that the factors make sense,
// ex. the grow factor must be >1.
map_error_t map_configure(map_t *map, float max_load_factor, float min_load_factor,
                          float grow_factor);// added *map argument

// Destroy the map, making sure you set the user's map pointer to NULL to
// avoid a dangling pointer.
map_error_t map_destroy(map_t **map);

// Insert a (key, value) pair into the map.
// Hash the key, find the right bucket, insert the key, value pair.
// Be careful: what if the key is already in the map?
// Check load factor and resize if necessary.
map_error_t map_insert(map_t *map, void *key, void *value);

// Retrieve value based on a key from the map.
map_error_t map_get(const map_t *map, void *key, void **out_value);

// Remove a (key, value) pair from the map.
// Check load factor and resize if necessary.
map_error_t map_remove(map_t *map, void *key);

//--------
// Map iterators.
// Iterators allow users to go through the map element-by-element on their
// own. For example, consider a user that wants to collect all the map
// values into a separate structure. Or another user that wants to multiply
// all map values by 10.

// Initialize the iterator to the first valid map element. Handle the empty
// map edge case.
map_error_t map_iter_start(const map_t *map, map_iterator_t *iter);

// Return the (key, value) pair at the current iterator location, then
// advance the iterator. Reason about whether to return copies of the
// key/value, or to return the internal pointers to the key/value.
map_error_t map_iter_next(const map_t *map, map_iterator_t *iter,
                          void **out_key, void **out_value);
//--------

// Pretty printing.
map_error_t map_print(const map_t *map);
