#include "query.h"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
START_TEST (test_query_is_reachable)
{
    uint32_t counter = 1;
    Graph* graph=graph_create("test/input/grafo20.gra",5);
    bool res;
    uint32_t *vn=calloc(graph->num_nodes, sizeof(uint32_t)); 
    res=find_path_reachability(0, 10, graph, vn, counter++);
    ck_assert_msg(res==true,"0 should be able to reach 10");

    res=find_path_reachability(4,12,graph,vn, counter++);
    ck_assert_msg(res==true,"4 should be able to reach 12");

    res=find_path_reachability(1,10,graph,vn, counter++);
    ck_assert_msg(res==false,"1 should not be able to reach 10");

    res=find_path_reachability(19,2,graph,vn, counter++);
    ck_assert_msg(res==false,"19 should not be able to reach 2");
}
END_TEST

Suite* query_suite(void)
{
    Suite* s;
    TCase* tc_core;
    s=suite_create("Graph");
    tc_core= tcase_create("Core");
    tcase_add_test(tc_core, test_query_is_reachable);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int n_fail;
    Suite* s;
    SRunner *sr;
    s=query_suite();
    sr=srunner_create(s);
    srunner_run_all(sr,CK_NORMAL);
    n_fail=srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail==0)?EXIT_SUCCESS:EXIT_FAILURE;
}
