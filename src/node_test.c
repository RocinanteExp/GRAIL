#include "Graph.h"
#include <check.h>
#include <stdlib.h>

START_TEST (test_node_create)
{
    Node* actual= node_create(3,0);
    ck_assert(actual->id==0);
    ck_assert(actual->children==NULL);
    ck_assert(actual->num_children==0);
    ck_assert(actual->intervals!=NULL);
    actual->intervals[0]=label_init(0,1);
    ck_assert(actual->intervals[0].left==0);
    ck_assert(actual->intervals[0].right==1);
    ck_assert(actual->interval_bitmap!=NULL);
}
END_TEST
START_TEST (test_node_set_childrens)
{
    Node* actual= node_create(3,0);
    char* str="0: 12 34 6 #";
    node_set_children(actual,str);
    ck_assert(actual->num_children==3);
    ck_assert(actual->children!=NULL && actual->children[0]==12);

}
END_TEST
Suite* node_suite(void)
{
    Suite* s;
    TCase* tc_core;
    s=suite_create("Node");
    tc_core= tcase_create("Core");
    tcase_add_test(tc_core, test_node_create);
    tcase_add_test(tc_core, test_node_set_childrens);
    suite_add_tcase(s, tc_core);
    return s;
}
int main(void){
    int n_fail;
    Suite* s;
    SRunner *sr;
    s=node_suite();
    sr=srunner_create(s);
    srunner_run_all(sr,CK_NORMAL);
    n_fail=srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail==0)?EXIT_SUCCESS:EXIT_FAILURE;
}