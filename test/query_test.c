#include "query.h"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
START_TEST (test_query_is_reachable)
{
    Graph* graph=graph_create("../test/input/grafo20.gra",5);
    bool res;
    Bitmap* vn=bitmap_create(20);
    res=query_is_reachable_multi(0,10,graph,vn);
    ck_assert_msg(res==true,"res should have been true but it was false");
    bitmap_clear_all(vn);
    res=query_is_reachable_multi(4,12,graph,vn);
    ck_assert_msg(res==true,"res should have been true but it was false");
    bitmap_clear_all(vn);
    res=query_is_reachable_multi(1,10,graph,vn);
    ck_assert_msg(res==false,"res should have been false but it was true");
    bitmap_clear_all(vn);
    res=query_is_reachable_multi(19,2,graph,vn);
    ck_assert_msg(res==false,"res should have been false but it was true");
}
END_TEST
START_TEST (test_query_init)
{
    Graph* graph=graph_create("../test/input/grafo20.gra",5);
    FILE* in;
    char buff[1000];
    int src,dest,res=0;
    query_init("../test/input/grafo20_25.que",graph);
    in=fopen("../test/input/grafo_risultati.txt","r");
    ck_assert_msg(in!=NULL,"File not found");
}
END_TEST
Suite* query_suite(void)
{
    Suite* s;
    TCase* tc_core;
    s=suite_create("Graph");
    tc_core= tcase_create("Core");
    tcase_add_test(tc_core, test_query_is_reachable);
    tcase_add_test(tc_core, test_query_init);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int n_fail;
    Suite* s;
    SRunner *sr;
    Graph *graph = graph_create("../test/input/grafo20.gra", 2);
    graph_print(graph, false, -1);
    s=query_suite();
    sr=srunner_create(s);
    srunner_run_all(sr,CK_NORMAL);
    n_fail=srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail==0)?EXIT_SUCCESS:EXIT_FAILURE;
}
