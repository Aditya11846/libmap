#include <map.h>
#include <map_internal.h>
#include <stdio.h>
#include <stdlib.h>

// Error logger.
void map_log_error(map_error_t err, const char *message) {
  fprintf(stderr, "[libmap ERROR] %s: %s\n", __map_error_str(err), message);
}

// Create Function.
map_error_t map_create(map_t **map,
                       void *(*usr_key_clone)(void *key),
                       void *(*usr_value_clone)(void *value),
                       uint64_t (*usr_hash)(void *key),
                       char *(*usr_stringify)(void *key, void *value),
                       int32_t (*usr_compare)(void *key1, void *key2),
                       void (*usr_free_key)(void *key),
                       void (*usr_free_value)(void *value)) {

    if (!map || !usr_key_clone || !usr_value_clone || !usr_hash || !usr_stringify ||
        !usr_compare || !usr_free_key || !usr_free_value) {
        return MAP_ERR_INVALID_ARG;
    }

    // Allocate memory for map struct
    *map = malloc(sizeof(map_t));
    if (*map == NULL) {
        return MAP_ERR_NO_MEM;
    }

    // Allocate memory for buckets
    (*map)->buckets = malloc(NUM_INITIAL_BUCKETS * sizeof(map_element_t*));
    if ((*map)->buckets == NULL) {
        free(*map);
        return MAP_ERR_NO_MEM;
    }

    // Initialize all buckets to NULL
    for (int i = 0; i < NUM_INITIAL_BUCKETS; i++) {
        (*map)->buckets[i] = NULL;
    }

    // Initialize map fields
    (*map)->num_buckets = NUM_INITIAL_BUCKETS;
    (*map)->num_entries = 0;
    (*map)->max_load_factor = 2.0;
    (*map)->min_load_factor = 0.5;
    (*map)->grow_factor = 2.0;
    (*map)->shrink_factor = 0.5;

    // Assign user functions
    (*map)->usr_key_clone = usr_key_clone;
    (*map)->usr_value_clone = usr_value_clone;
    (*map)->usr_hash = usr_hash;
    (*map)->usr_stringify = usr_stringify;
    (*map)->usr_compare = usr_compare;
    (*map)->usr_free_key = usr_free_key;
    (*map)->usr_free_value = usr_free_value;

    return MAP_OK;
}

// Insert Function
map_error_t map_insert(map_t *map, void *key, void *value) {
    if (!map || !key || !value) return MAP_ERR_INVALID_ARG;

    map_error_t result = __map_insert_no_resize(map, key, value);
    if (result != MAP_OK) return result;  // Propagate errors

    // Resize if needed
    if ((double)map->num_entries / map->num_buckets > map->max_load_factor) {
        result = __map_resize(map, map->grow_factor);
        if (result != MAP_OK) return result;
    }

    return MAP_OK;
}

// Print Function
map_error_t map_print(const map_t *map) {
   	if (map == NULL){
		printf("Map is null\n");
		return MAP_ERR_INVALID_ARG;
	}

	printf("Map contents:\n");
	for (int i=0; i < map->num_buckets; i++){
		map_element_t *current = map->buckets[i];
		printf("Buckets %d: ",i);

		while (current != NULL){
			char *entry_str = map->usr_stringify(current->_key, current->_value);
			if (entry_str == NULL){
				return MAP_ERR_UNKNOWN;
			}

			printf("%s", entry_str);
			free(entry_str);

			current = current->_next;
		}
		printf("\n");
	}
	return MAP_OK;

}

// Get Function
map_error_t map_get(const map_t *map, void *key, void **value) {
    // 1. Validate Inputs.
    if (map == NULL || key == NULL || value == NULL) {
        return MAP_ERR_INVALID_ARG;
    }

    // 2. Hashing the key to find correct bucket.
    uint64_t hash = map->usr_hash(key);
    int index = hash % map->num_buckets;

    // Traverse the bucket's linked list.
    map_element_t *current = map->buckets[index];

    while (current != NULL) {
        int cmp_result = map->usr_compare(current->_key, key);

        // Handle broken usr_compare function
        if (cmp_result < -1 || cmp_result > 1) {
            return MAP_ERR_UNKNOWN;
        }

        if (cmp_result == 0) {
            *value = current->_value; // Key found, set value.
            return MAP_OK;
        }
        current = current->_next; // Move to the next element
    }

    // If key not found
    return MAP_ERR_NOT_FOUND;
}


// Remove Function
map_error_t map_remove(map_t *map, void *key){
	if (map == NULL || key == NULL) {
		return MAP_ERR_INVALID_ARG;
	}
	// Hashing the key
	int64_t hash = map->usr_hash(key);
	int index = hash % map->num_buckets; // Getting the index

	// Current and previous pointers

	map_element_t *current = map->buckets[index];
	map_element_t *prev = NULL;

	// Linked list traversal

	while (current != NULL){
		if (map->usr_compare(key,current->_key) == 0) {

			// Key found remove node (2 cases head of the list or not)

			if (prev == NULL) { // case 1: Node to be removed is at the head of the list
				map->buckets[index] = current->_next;
			}

			else {
				prev->_next = current->_next;
			}

			map->usr_free_key(current->_key);
			map->usr_free_value(current->_value);

			free(current);// freeing the node itself
			map->num_entries--;// Decrement the number of entries in the map

			//Resize if necessary

			double load_factor = (double)map->num_entries / map->num_buckets;
			if (load_factor < map->min_load_factor && map->num_buckets > 1) { // Checking if map needs to be resized and has multiple buckets

				__map_resize(map,map->shrink_factor);
			}

			return MAP_OK; // Deletion Successful!
		}
	prev = current;
	current = prev->_next;
	}
	return MAP_ERR_NOT_FOUND; // Key not found
}


// Destroy Function
map_error_t map_destroy(map_t **map){
	if (map == NULL || *map == NULL) {
		return MAP_ERR_INVALID_ARG;
	}

	// Loop through the buckets

	for ( int i = 0; i < (*map)->num_buckets; i++) {
		map_element_t *current = (*map)->buckets[i];
		// Loop through the linked list
		while (current != NULL) {
			map_element_t *next = current->_next;

			(*map)->usr_free_key(current->_key);
			(*map)->usr_free_value(current->_value);

			free(current);
			current = next;
		}
	}

	free((*map)->buckets); // Free buckets array
	free(*map);
	*map = NULL; // Avoids dangling pointer

	return MAP_OK;

}

// Configure Function
map_error_t map_configure(map_t *map, float max_load_factor, float min_load_factor, float grow_factor) {
	if (map == NULL || max_load_factor <= 0 || min_load_factor <0 || grow_factor <= 1 || min_load_factor >= max_load_factor) {
		return MAP_ERR_INVALID_ARG;
	}

	map->max_load_factor = max_load_factor;
	map->min_load_factor = min_load_factor;
	map->grow_factor = grow_factor;

	return MAP_OK;
}

// Iterator Start Function
map_error_t map_iter_start(const map_t *map, map_iterator_t *iter) {
	if (map == NULL || iter == NULL) {
		return MAP_ERR_INVALID_ARG;
	}

	iter->current_bucket = -1; //Not zero since first index is 0
	iter->current_element = NULL;

	for (int i = 0; i < map->num_buckets; i++){
		if(map->buckets[i] != NULL) {
			//Get the first bucket that points to an element
			iter->current_bucket = i; // Properly set current bucket
			iter->current_element = map->buckets[i];
			return MAP_OK; // First element found
		}
	}

	return MAP_ERR_END_OF_MAP;// Map is empty
}


// Iterator Next Function
map_error_t map_iter_next(const map_t *map, map_iterator_t *iter, void **out_key, void **out_value) {

    if (map == NULL || iter == NULL || out_key == NULL || out_value == NULL) {
        return MAP_ERR_INVALID_ARG;
    }

    // If current element exists, use it first
    if (iter->current_element != NULL) {
        *out_key = iter->current_element->_key;
        *out_value = iter->current_element->_value;

        // Move to next element in the chain for future calls
        iter->current_element = iter->current_element->_next;
        return MAP_OK;
    }

    // If current chain is done, move to the next bucket
    while (iter->current_bucket + 1 < map->num_buckets) {
        iter->current_bucket++;
        iter->current_element = map->buckets[iter->current_bucket];

        if (iter->current_element != NULL) {
            *out_key = iter->current_element->_key;
            *out_value = iter->current_element->_value;

            // Prepare for the next call
            iter->current_element = iter->current_element->_next;
            return MAP_OK;
        }
    }

    return MAP_ERR_END_OF_MAP; // All elements iterated
}


// Map Size Getting Function
map_error_t map_get_size (map_t *map, int *num_elements) {

	if (map == NULL || num_elements == NULL) {
		return MAP_ERR_INVALID_ARG;

	}

	*num_elements = map->num_entries;
	return MAP_OK;
}

// Map Buckets Getting Function
map_error_t map_get_num_buckets (map_t *map, int *num_buckets) {

	if (map == NULL || num_buckets == NULL) {
		return MAP_ERR_INVALID_ARG;
	}

	*num_buckets = map->num_buckets;
	return MAP_OK;
}
