#pragma once

typedef struct map_element {
  void *_key;
  void *_value;
  struct map_element *_next;
} map_element_t;

typedef struct {
  map_element_t **buckets;
  int32_t num_buckets;
  int32_t num_entries;
  float max_load_factor; // Set this to 2.0
  float min_load_factor; // Set this to max_load_factor/4.
  float grow_factor;     // Grow num_buckets by 2.0x on resize.
  float shrink_factor;   // Shrink num_buckets by 0.5x on resize.

  // User provided functions.
  void *(*usr_key_clone)(void *key);
  void *(*usr_value_clone)(void *value);
  uint64_t (*usr_hash)(void *key);
  char *(*usr_stringify)(void *key, void *data);
  int32_t (*usr_compare)(void *key1, void *key2);
  void (*usr_free_key)(void *key);
  void (*usr_free_value)(void *value);
} map_t;

typedef struct {
  int32_t current_bucket;         // Which bucket index we're on.
  map_element_t *current_element; // Which element in the chain.
} map_iterator_t;

typedef enum {
  MAP_OK = 0,          // Operation succeeded
  MAP_ERR_NO_MEM,      // Memory allocation failed
  MAP_ERR_NOT_FOUND,   // Requested key not found
  MAP_ERR_INVALID_ARG, // One or more arguments were invalid
  MAP_ERR_OVERFLOW,    // For example, table too large to resize
  MAP_ERR_END_OF_MAP,
  MAP_ERR_UNKNOWN // Catch-all for other errors
} map_error_t;
