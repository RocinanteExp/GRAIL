#include "label.h"
#include <check.h>
#include <stdlib.h>
START_TEST (test_label_init)
    {
        Label l1=label_init(0,7);
        ck_assert_int_eq(0,l1.left);
        ck_assert_int_eq(7,l1.right);
    }
END_TEST
START_TEST (test_label_include)
{
    Label l1=label_init(1,9);
    Label l2=label_init(2,7);
    bool actual;
    actual=label_include(l2,l1);
    ck_assert(actual==true);
    actual=label_include(l1,l2);
    ck_assert(actual==false);
    l1.left=3;
    actual=label_include(l1,l2);
    ck_assert(actual==false);
    l1.left=1;
    l1.right=6;
    actual=label_include(l1,l2);
    ck_assert(actual==false);
}
END_TEST 
Suite* label_suite(void)
{
    Suite* s;
    TCase* tc_core;
    s=suite_create("Label");
    tc_core= tcase_create("Core");
    tcase_add_test(tc_core, test_label_init);
    tcase_add_test(tc_core,test_label_include);
    suite_add_tcase(s, tc_core);
    return s;
}
int main(void){
    int n_fail;
    Suite* s;
    SRunner *sr;
    s=label_suite();
    sr=srunner_create(s);
    srunner_run_all(sr,CK_NORMAL);
    n_fail=srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail==0)?EXIT_SUCCESS:EXIT_FAILURE;
}
