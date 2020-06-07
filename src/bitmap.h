#ifndef BITMAP_H
#define BITMAP_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * bitset is the actual bitmap
 * length is the length of the array
 * num_bits is the number of bits logically available in the bitmap
 * */
typedef struct bitmap_s {
    uint32_t* bitset;
    size_t length;
    size_t num_bits;
}Bitmap;

// allocate a bitmap struct. The bitset array is cleared to 0 
Bitmap* bitmap_create(size_t num_bits);
// set the bit at <pos> to 1
void bitmap_set_bit(Bitmap* bitmap, size_t pos);
// set the bit at <pos> to 0
void bitmap_clear_bit(Bitmap* bitmap, size_t pos);
/*
 * test the bit at pos.
 * Return 1 if the bit is set, 0 if the bit is clear, -1 if the pos is out of boundary
 */ 
int bitmap_test_bit(Bitmap* bitmap, size_t pos);
// set all the bits to 1
void bitmap_set_all(Bitmap* bitmap);
// set all the bits to 0
void bitmap_clear_all(Bitmap* bitmap);
// free a bitmap structure
void bitmap_destroy(Bitmap* bitmap);

#endif
