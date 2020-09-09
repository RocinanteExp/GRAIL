#include "graph.h"
#include <check.h>
#include <stdlib.h>

START_TEST (test_node_create)
{
    Node* n = node_create(3, 0);
    ck_assert_msg(n->id == 0, "node is not 0, but %d", n->id);
    ck_assert(n->children == NULL);
    ck_assert_msg(n->num_children == 0, "node has %d children instead of 0", n->num_children);
    ck_assert(n->intervals != NULL);
    ck_assert_msg(n->num_intervals == 3, "node has %d labels instead of 3", n->num_intervals);
    ck_assert_msg(n->intervals[0].left == UINT32_MAX, "n->intervals[0].left is not UINT32_MAX");
    ck_assert_msg(n->intervals[0].right == UINT32_MAX, "n->intervals[0].right is not UINT32_MAX");
    ck_assert_msg(n->intervals[1].left == UINT32_MAX, "n->intervals[1].left is not UINT32_MAX");
    ck_assert_msg(n->intervals[1].right == UINT32_MAX,"n->intervals[1].right is not UINT32_MAX");
    ck_assert_msg(n->intervals[2].left == UINT32_MAX, "n->intervals[2].left is not UINT32_MAX");
    ck_assert_msg(n->intervals[2].right == UINT32_MAX, "n->intervals[2].right is not UINT32_MAX");
}
END_TEST

Suite* node_suite(void)
{
    Suite* s;
    TCase* tc_core;
    s=suite_create("Test Suite Node");
    tc_core= tcase_create("Core");
    tcase_add_test(tc_core, test_node_create);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void){
    int n_fail;
    Suite* s;
    SRunner* sr;
    s=node_suite();
    sr=srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    n_fail=srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail == 0 ? EXIT_SUCCESS:EXIT_FAILURE);
}
