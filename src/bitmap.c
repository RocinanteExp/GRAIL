#include <stdlib.h>
#include <string.h>
#include "bitmap.h"

// sizeof(uint32_t) * 8
const static uint32_t SIZE_CHUNK_IN_BITS = 32;
const static uint32_t SHIFT_AMOUNT = 5;

Bitmap* bitmap_create(size_t num_bits){
   //computing the ceiling of num_bits / SIZE_CHUNK_IN_BITS 
   size_t array_length = (num_bits + SIZE_CHUNK_IN_BITS - 1) >> SHIFT_AMOUNT; 

   Bitmap* ret_bitmap = malloc(sizeof(Bitmap));
   if(ret_bitmap == NULL){
       return NULL;
   }

   ret_bitmap->bitset = calloc(array_length, sizeof(uint32_t));
   if(ret_bitmap->bitset == NULL){
        free(ret_bitmap);
        return NULL;
   }

   ret_bitmap->num_bits = num_bits;
   ret_bitmap->length = array_length;

   return ret_bitmap;

}

void bitmap_set_bit(Bitmap* bitmap, size_t pos){
   
   if((pos < 0) || (pos >= bitmap->num_bits))
       return;

   const uint32_t index = pos >> SHIFT_AMOUNT;
   const uint32_t offset = pos & 31;
   bitmap->bitset[index] |= 1 << (31 - offset); 

}

void bitmap_clear_bit(Bitmap* bitmap, size_t pos){

   if(pos < 0 || pos >= bitmap->num_bits)
       return;
   
   const uint32_t index = pos >> SHIFT_AMOUNT;
   const uint32_t offset = pos & 31;
   bitmap->bitset[index] &= ~(1 << (31 - offset)); 

}

int bitmap_test_bit(Bitmap* bitmap, size_t pos){

    if((pos < 0) || (pos >= bitmap->num_bits))
        return -1;

    const uint32_t index = pos >> SHIFT_AMOUNT;
    const uint32_t offset = pos & 31;
    
    uint32_t flag = 1 << (31 - offset);

    if((bitmap->bitset[index] & flag) != 0)
        return 1;

    return 0;

}

void bitmap_set_all(Bitmap* bitmap){
    
   int allOnes = -1;

   for(int i = 0; i < bitmap->length; i++){
        bitmap->bitset[i] = allOnes;
   }

}

void bitmap_clear_all(Bitmap* bitmap){
    
   const uint32_t allZeros = 0;

   //memset(bitmap->bitset, 0, sizeof(uint32_t) * bitmap->length);
   for(int i = 0; i < bitmap->length; i++){
        bitmap->bitset[i] = allZeros;
   }

}

void bitmap_destroy(Bitmap* bitmap){
    
    free(bitmap->bitset);
    free(bitmap);

}
