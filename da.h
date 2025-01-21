#ifndef DA_H_
#define DA_H_

#include <stdlib.h>
#include <string.h>

#ifndef DA_INIT_CAPACITY
#define DA_INIT_CAPACITY 256
#endif // DA_INIT_CAPACITY

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} DA;

DA DA_create();
void DA_init(DA *da);
void DA_extend(DA *da, size_t count);
void DA_append(DA *da, char *items, size_t count);

#endif // DA_H_

#pragma message("DA_IMPLEMENTATION should be deleted.")
#define DA_IMPLEMENTATION
#ifdef DA_IMPLEMENTATION

DA DA_create() {
    return (DA){
        .items = (char *)malloc(DA_INIT_CAPACITY),
        .capacity = DA_INIT_CAPACITY,
        .count = 0
    };
}

void DA_init(DA *da) {
    da->items = (char *)malloc(DA_INIT_CAPACITY);
    da->capacity = DA_INIT_CAPACITY;
}

void DA_extend(DA *da, size_t count) {
    // TODO: Check result of memory allocations
    // TODO: Increase size to a power of 2?
    if (da->capacity > da->count + count)
        return;
    da->capacity = da->count + count;
    da->items = (char *)realloc(da->items, da->capacity);
}

void DA_append(DA *da, char *items, size_t count) {
    // TODO: Check result of memory allocations
    if (count > da->capacity - da->count) {
        size_t new_cap = da->capacity;
        while (new_cap <= da->count + count)
            new_cap *= 2;
        DA_extend(da, new_cap - da->count);
    }
    memcpy(da->items + da->count, items, count);
    da->count += count;
}

#endif // DA_IMPLEMENTATION