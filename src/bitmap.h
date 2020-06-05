#ifndef BITMAP_H
#define BITMAP_H
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    int* bitset;
    size_t length;
    size_t num_bits;
}bitmap;

// allocate a bitmap struct. The bitset array is cleared to 0 
bitmap* bitmap_create(size_t nBits);
// set the bit at <pos> to 1
void bitmap_set_bit(bitmap* bitmap, size_t pos);
// set the bit at <pos> to 0
void bitmap_clear_bit(bitmap* bitmap, size_t pos);
/*
 * test the bit at pos.
 * Return 1 if the bit is set, 0 if the bit is clear, -1 if the pos is out of boundary
 */ 
int bitmap_test_bit(bitmap* bitmap, size_t pos);
// set all the bits to 1
void bitmap_set_all(bitmap* bitmap);
// set all the bits to 0
void bitmap_clear_all(bitmap* bitmap);
// free a bitmap structure
void bitmap_destroy(bitmap* bitmap);

#endif
