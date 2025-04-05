// Header file for private structs and functions.
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <map.h>
#include <types.h>

const char* __map_error_str(map_error_t err);

map_error_t __map_insert_no_resize(map_t *map, void *key, void *value);
map_error_t __map_resize(map_t *map, float resize_factor);
