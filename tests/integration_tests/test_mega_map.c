#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map.h>

// Stringify function for string keys and integer values
char* user_stringify(void *key, void *value) {
        char *k = (char *)key;
        int *v = (int *)value;
        char *str = malloc(100 * sizeof(char));
        if (str != NULL) {
                snprintf(str, 100, "(Key: %s, Value: %d)", k, *v);
        }
        return str;
}

// Hash function for string keys (djb2 algorithm)
uint64_t user_hash(void *key) {
        char *str = (char *)key;
        uint64_t hash = 5381;
        int c;

        while ((c = *str++)) {
                hash = ((hash << 5) + hash) + c;
        }

        return hash;
}

// Key comparison
int32_t user_compare(void *key1, void *key2) {
        return strcmp((char *)key1, (char *)key2);
}

// Clone key (for strings)
void* user_key_clone(void *key) {
		char *copy = malloc(strlen((char*)key) + 1);
		if (copy) {
			strcpy(copy, (char*)key);
		}
		return copy;
}

// Clone value (for integers)
void* user_value_clone(void *value) {
        int *new_value = malloc(sizeof(int));
        *new_value = *((int *)value);
        return new_value;
}

// Free functions
void user_free_key(void *key) {
        free(key);
}

void user_free_value(void *value) {
        free(value);
}

int main(void) {

        map_t *map;

        // Create the map
        printf("Creating map...\n");
        if (map_create(&map, user_key_clone, user_value_clone, user_hash, user_stringify, user_compare, user_free_key, user_free_value) != MAP_OK) {
                printf("Failed to create map.\n");
                return 1;
        }

        // Insert key-value pairs
        printf("\nInserting key-value pairs...\n");
        char *keys[] = {"apple", "banana", "cherry", "date", "elderberry", "banana", "dragonfruit", "kiwi"};
        int values[] = {10, 20, 30, 40, 50, 60, 70, 80};

        for (int i = 0; i < 5; i++) {
                if (map_insert(map, keys[i], &values[i]) != MAP_OK) {
                        printf("Failed to insert key: %s\n", keys[i]);
                }
        }

        // Print map after insertion
        printf("\n--- Map After Insertion ---\n");
        map_print(map);

        // Get specific key
        printf("\nRetrieving value for key 'cherry':\n");
        void *retrieved_value = NULL;

        if (map_get(map, "cherry", &retrieved_value) == MAP_OK) {
                printf("Found: cherry -> %d\n", *((int *)retrieved_value));
        } else {
                printf("Key 'cherry' not found.\n");
        }

        // Remove a key
        printf("\nRemoving key 'banana'...\n");

        if (map_remove(map, "banana") == MAP_OK) {
                printf("Key 'banana' removed.\n");
        } else {
                printf("Failed to remove key 'banana'.\n");
        }

        // Print map after removal
        printf("\n--- Map After Removal ---\n");
        map_print(map);

        // Iterate over the map
        printf("\n--- Iterating Over Map ---\n");
        map_iterator_t iter;
        void *key;
        void *value;

        if (map_iter_start(map, &iter) == MAP_OK) {
                while (map_iter_next(map, &iter, &key, &value) == MAP_OK) {
                        printf("Key: %s, Value: %d\n", (char *)key, *((int *)value));
                }

        } else {
                printf("Map is empty.\n");
        }

        // Destroy the map
        printf("\nDestroying map...\n");

        if (map_destroy(&map) == MAP_OK) {
                printf("Map successfully destroyed.\n");
        } else {
                printf("Failed to destroy map.\n");
        }

        return 0;
}

