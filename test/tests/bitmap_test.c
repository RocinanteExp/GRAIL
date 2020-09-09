#include "bitmap.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
static int SIZE_CHUNK = (sizeof(int) * 8);
START_TEST (test_bitmap_create)
{
    Bitmap* bitmap=NULL;
    bitmap = bitmap_create(24);
    ck_assert(bitmap!=NULL);
    ck_assert(bitmap->bitset!=NULL);
    ck_assert(bitmap->length==1 ||bitmap->length==2);
    ck_assert(bitmap->num_bits==24);

}
END_TEST

START_TEST (test_bitmap_set_bit)
{
    Bitmap* bitmap=NULL;
    bitmap = bitmap_create(32);
    int pos=0;
    bitmap_set_bit(bitmap,0);
    ck_assert( (bitmap->bitset[pos / SIZE_CHUNK] & 1 << (pos % SIZE_CHUNK))== (1 << (pos % SIZE_CHUNK)));
    pos=25;
    bitmap_set_bit(bitmap,pos);
    ck_assert( (bitmap->bitset[pos / SIZE_CHUNK] & 1 << (pos % SIZE_CHUNK))== (1 << (pos % SIZE_CHUNK)));

}
END_TEST

START_TEST (test_Bitmap_clear_bit)
{
    Bitmap* bitmap=NULL;
    bitmap = bitmap_create(32);
    int pos=0;
    bitmap->bitset[0]=1;
    ck_assert( (bitmap->bitset[pos / SIZE_CHUNK] & 1 << (pos % SIZE_CHUNK))== (1 << (pos % SIZE_CHUNK)));
    Bitmap_clear_bit(bitmap,pos);
    ck_assert( (bitmap->bitset[pos / SIZE_CHUNK] & 1 << (pos % SIZE_CHUNK))== 0);

}
END_TEST

START_TEST (test_bitmap_set_all)
{
    Bitmap* bitmap=NULL;
    bitmap = bitmap_create(32);
    bitmap_set_all(bitmap);
    ck_assert(bitmap->bitset[0]==-1);
}
END_TEST
START_TEST (test_bitmap_clear_all)
{
    Bitmap* bitmap=NULL;
    bitmap = bitmap_create(32);
    bitmap_clear_all(bitmap);
    ck_assert(bitmap->bitset[0]==0);
}
END_TEST
START_TEST (test_bitmap_destory)
{
    Bitmap* bitmap=NULL;
    bitmap = bitmap_create(32);
    bitmap_destroy(bitmap);
}
END_TEST

START_TEST (test_bitmap_test_bit)
{
    Bitmap* bitmap=NULL;
    bitmap = bitmap_create(32);
    bitmap->bitset[0]=1;
    ck_assert(bitmap_test_bit(bitmap,0)==1);
    bitmap->bitset[0]=0;
    ck_assert(bitmap_test_bit(bitmap,0)==0);
    ck_assert(bitmap_test_bit(bitmap,40)==-1);
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
    tcase_add_test(tc_core, test_Bitmap_clear_bit);
    tcase_add_test(tc_core, test_bitmap_destory);
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
