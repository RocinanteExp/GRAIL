#include <stdlib.h>
#include "bitmap.h"

const static int SIZE_CHUNK = (sizeof(int) * 8);

Bitmap* bitmap_create(size_t num_bits){
   //computing the ceiling of num_bits / SIZE_CHUNK 
   size_t array_length = (num_bits + SIZE_CHUNK - 1) / SIZE_CHUNK;

   Bitmap* ret_bitmap = malloc(sizeof(Bitmap));
   if(ret_bitmap == NULL){
       return NULL;
   }

   ret_bitmap->bitset = calloc(array_length, SIZE_CHUNK);
   if(ret_bitmap->bitset == NULL){
        free(ret_bitmap);
        return NULL;
   }

   ret_bitmap->num_bits = num_bits;
   ret_bitmap->length = array_length;

   return ret_bitmap;

}

void bitmap_set_bit(Bitmap* bitmap, size_t pos){
   
   if(pos < 0 || pos >= bitmap->num_bits)
       return;

   bitmap->bitset[pos / SIZE_CHUNK] |= 1 << (pos % SIZE_CHUNK); 

}

void Bitmap_clear_bit(Bitmap* bitmap, size_t pos){

   if(pos < 0 || pos >= bitmap->num_bits)
       return;
   
   bitmap->bitset[pos / SIZE_CHUNK] &= 0 << (pos % SIZE_CHUNK); 

}

int bitmap_test_bit(Bitmap* bitmap, size_t pos){

    if(pos < 0 || pos >= bitmap->num_bits)
        return -1;

    int index = pos / SIZE_CHUNK;
    int offset = pos % SIZE_CHUNK;
    unsigned int flag = 1;
    
    flag = flag << offset;

    if(bitmap->bitset[index] & flag)
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
    
   int allZeros = 0;

   for(int i = 0; i < bitmap->length; i++){
        bitmap->bitset[i] = allZeros;
   }

}

void bitmap_destroy(Bitmap* bitmap){
    
    free(bitmap->bitset);
    free(bitmap);

}
