#include <map.h>
#include <map_internal.h>

// Convert error enum to string. C doesn't have reflection,
// making approaches like this necessary.
const char *__map_error_str(map_error_t err) {
  switch (err) {
  case MAP_OK:
    return "MAP_OK";
  case MAP_ERR_NO_MEM:
    return "MAP_ERR_NO_MEM";
  case MAP_ERR_NOT_FOUND:
    return "MAP_ERR_NOT_FOUND";
  case MAP_ERR_INVALID_ARG:
    return "MAP_ERR_INVALID_ARG";
  case MAP_ERR_OVERFLOW:
    return "MAP_ERR_OVERFLOW";
  case MAP_ERR_END_OF_MAP:
    return "MAP_ERR_END_OF_MAP";
  default:
    return "MAP_ERR_UNKNOWN";
  }
}

// Map insert no resize
map_error_t __map_insert_no_resize(map_t *map, void *key, void *value) {
    if (!map || !key || !value) return MAP_ERR_INVALID_ARG;

    // Calculate bucket index using hash function
    uint64_t hash = map->usr_hash(key);
    size_t index = hash % map->num_buckets;

    // Check if the key already exists
    map_element_t *current = map->buckets[index];
    while (current) {
        if (map->usr_compare(current->_key, key) == 0) {
            // Key exists, update value
            map->usr_free_value(current->_value);
            current->_value = map->usr_value_clone(value);
            if (!current->_value) return MAP_ERR_NO_MEM;
            return MAP_OK;
        }
        current = current->_next;
    }

    // Create new element
    map_element_t *new_elem = malloc(sizeof(map_element_t));
    if (!new_elem) return MAP_ERR_NO_MEM;

    new_elem->_key = map->usr_key_clone(key);
    if (!new_elem->_key) {  // Key clone failed
        free(new_elem);
        return MAP_ERR_NO_MEM;
    }

    new_elem->_value = map->usr_value_clone(value);
    if (!new_elem->_value) {  // Value clone failed
        map->usr_free_key(new_elem->_key);
        free(new_elem);
        return MAP_ERR_NO_MEM;
    }

    // Insert into bucket
    new_elem->_next = map->buckets[index];
    map->buckets[index] = new_elem;
    map->num_entries++;

    return MAP_OK;
}

// Map Resizing
map_error_t __map_resize(map_t *map, float resize_factor){

	if (map == NULL || resize_factor <= 0) {
		return MAP_ERR_INVALID_ARG;

	}

	uint64_t new_num_buckets = (uint64_t)(map->num_buckets * resize_factor);
	if (new_num_buckets < 1) { // Atleast one bucket must exist
		return MAP_ERR_INVALID_ARG;
	}

	// Allocating new buckets
	map_element_t **new_buckets = malloc(new_num_buckets * sizeof(map_element_t *));

	if (new_buckets == NULL) {
	return MAP_ERR_NO_MEM;
	}
	// all buckets->NULL
	for (uint64_t i = 0; i < new_num_buckets; i++) {
		new_buckets[i] = NULL;

	}

	// Going through the old buckets
	for (int i = 0; i < map->num_buckets; i++) {
		map_element_t *current = map->buckets[i];
		while (current != NULL) {
			map_element_t *next = current->_next;
			uint64_t new_index = map->usr_hash(current->_key) % new_num_buckets;

			// Insert into new bucket
			current->_next = new_buckets[new_index];
			new_buckets[new_index] = current;
			current = next;
		}

	}
	// Free old buckets not elements as elements have been moved
	free(map->buckets);
	map->buckets = new_buckets;
	map->num_buckets = new_num_buckets;

	return MAP_OK;


}

