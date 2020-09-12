#include "bitmap.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>

static int SIZE_CHUNK_IN_BITS = (sizeof(uint32_t) * 8);
START_TEST (test_bitmap_create)
{
    Bitmap *bitmap = bitmap_create(50);

    ck_assert(bitmap != NULL);
    ck_assert(bitmap->bitset != NULL);
    ck_assert(bitmap->length == 2);
    ck_assert(bitmap->num_bits == 50);

    bitmap_destroy(bitmap);
}
END_TEST

START_TEST (test_bitmap_set_bit)
{
    Bitmap *bitmap = NULL;
    bitmap = bitmap_create(64);

    bitmap_set_bit(bitmap, 0);
    ck_assert(bitmap->bitset[0] == (1 << 31));

    bitmap_set_bit(bitmap, 3);
    ck_assert(bitmap->bitset[0] == (1 << 28) + (1 << 31));

    bitmap_set_bit(bitmap, 37);
    ck_assert(bitmap->bitset[1] ==  1 << 26);

    bitmap_clear_all(bitmap);
    bitmap_set_bit(bitmap, 32);
    ck_assert(bitmap->bitset[1] ==  (1 << 31) );

    bitmap_set_bit(bitmap, 35);
    ck_assert(bitmap->bitset[1] ==  (1 << 31) + (1 << 28));

    bitmap_clear_all(bitmap);
    bitmap_set_bit(bitmap, 63);
    ck_assert(bitmap->bitset[1] == 1); 

    bitmap_set_bit(bitmap, 61);
    ck_assert(bitmap->bitset[1] == 5);

    bitmap_set_bit(bitmap, 59);
    ck_assert(bitmap->bitset[1] == 21);

    bitmap_destroy(bitmap);
}
END_TEST

START_TEST (test_bitmap_clear_bit)
{
    Bitmap *bitmap = NULL;
    bitmap = bitmap_create(33);

    bitmap->bitset[0] = 7;
    bitmap->bitset[1] = 1;
    ck_assert((bitmap->bitset[0]) == 7);
    ck_assert(bitmap->bitset[1] == 1);

    bitmap_clear_bit(bitmap, 30);
    ck_assert(bitmap->bitset[0] == 5);

    bitmap_clear_bit(bitmap, 31);
    ck_assert(bitmap->bitset[0] == 4);
    
    bitmap_clear_bit(bitmap, 29);
    ck_assert(bitmap->bitset[0] == 0);

    bitmap_destroy(bitmap);
}
END_TEST

START_TEST (test_bitmap_set_all)
{
    Bitmap* bitmap = NULL;
    bitmap = bitmap_create(35);

    bitmap_set_all(bitmap);
    ck_assert(bitmap->bitset[0] == -1);
    ck_assert(bitmap->bitset[1] == -1);
    ck_assert(bitmap_test_bit(bitmap, 30) == 1);
    ck_assert(bitmap_test_bit(bitmap, 34) == 1);
    ck_assert(bitmap_test_bit(bitmap, 2) == 1);
    ck_assert(bitmap_test_bit(bitmap, 5) == 1);

    bitmap_destroy(bitmap);
}

END_TEST
START_TEST (test_bitmap_clear_all)
{
    Bitmap* bitmap = NULL;
    bitmap = bitmap_create(39);
    bitmap_set_bit(bitmap, 2);
    bitmap_set_bit(bitmap, 38);
    bitmap_clear_all(bitmap);
    ck_assert(bitmap->bitset[0] == 0);
    ck_assert(bitmap->bitset[1] == 0);

    bitmap_destroy(bitmap);
}
END_TEST

START_TEST (test_bitmap_test_bit)
{
    Bitmap *bitmap = NULL;
    bitmap = bitmap_create(32);
    bitmap->bitset[0] = (~0 - 5);

    ck_assert(bitmap_test_bit(bitmap, 31) == 0);
    ck_assert(bitmap_test_bit(bitmap, 29) == 0);
    ck_assert(bitmap_test_bit(bitmap, 0) == 1);
    ck_assert(bitmap_test_bit(bitmap, 1) == 1);
}
END_TEST

Suite* bitmap_suite(void)
{
    Suite* s;
    TCase* tc_core;
    s=suite_create("Bitmap");
    tc_core= tcase_create("Core");
    tcase_add_test(tc_core, test_bitmap_create);
    tcase_add_test(tc_core, test_bitmap_clear_all);
    tcase_add_test(tc_core, test_bitmap_set_all);
    tcase_add_test(tc_core, test_bitmap_set_bit);
    tcase_add_test(tc_core, test_bitmap_test_bit);
    tcase_add_test(tc_core, test_bitmap_clear_bit);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int n_fail;
    Suite* s;
    SRunner *sr;
    s=bitmap_suite();
    sr=srunner_create(s);
    srunner_run_all(sr,CK_NORMAL);
    n_fail=srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail==0)?EXIT_SUCCESS:EXIT_FAILURE;
}
