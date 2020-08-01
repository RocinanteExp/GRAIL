#include "label.h"
#include <check.h>
#include <stdlib.h>

START_TEST (test_label_init)
{
    Label right = label_init(0, 7);
    ck_assert_int_eq(0, right.left);
    ck_assert_int_eq(7, right.right);
}
END_TEST

START_TEST (test_label_include)
{
    Label left, right;
    bool res;

    left = label_init(1, 9);
    right = label_init(1, 9);
    res = label_include(left, right);
    ck_assert_msg(
            res == true, 
            "left [%d, %d] should be included in right [%d, %d]", left.left, left.right, right.left, right.right
            );

    left = label_init(2,8);
    right = label_init(1,9);
    res = label_include(left,right);
    ck_assert_msg(
            res == true, 
            "left [%d, %d] should be included in right [%d, %d]", left.left, left.right, right.left, right.right
            );

    left = label_init(1,10);
    right = label_init(1,9);
    res = label_include(left,right);
    ck_assert_msg(
            res == false, 
            "left [%d, %d] should not be included in right [%d, %d]", left.left, left.right, right.left, right.right
            );

    left = label_init(1, 9);
    right = label_init(2, 9);
    res = label_include(left,right);
    ck_assert_msg(
            res == false, 
            "left [%d, %d] should not be included in right [%d, %d]", left.left, left.right, right.left, right.right
            );

    left = label_init(1, 10);
    right = label_init(2, 9);
    res = label_include(left,right);
    ck_assert_msg(
            res == false, 
            "left [%d, %d] should not be included in right [%d, %d]", left.left, left.right, right.left, right.right
            );
}
END_TEST 

START_TEST (test_ramdom_shuffle)
{
    uint32_t vec[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    random_shuffle(vec, 10);

    bool is_all = true;
    bool found = false;

    for(int i = 0; i < 10; ++i) {
        for(int j = 0; j < 10 && !found; ++j) {
            if(vec[j] == i) {
                found = true;
            }
        }

        if(found == false) {
            is_all = false;
            break;
        }
    }

    if(!is_all)
        ck_abort_msg("random vec does not contain all the elements");
}
END_TEST

Suite* label_suite(void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create("Test Suite Label");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_label_init);
    tcase_add_test(tc_core, test_label_include);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void){
    int n_fail;
    Suite* s;
    SRunner* sr;
    s = label_suite();
    sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    n_fail = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail == 0 ? EXIT_SUCCESS:EXIT_FAILURE);
}
