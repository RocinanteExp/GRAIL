#include "menu.h"
#include "graph.h"
#include "label.h"
#include "query.h"
#include "constants.h"

void cust_exit(Graph *graph) {
    printf("Thank you for using our program\n");
    query_cleanup();
    graph_destroy(graph);
};

void menu(int argc, char **argv) {
    if(argc < 4 || argc > 6) {
        printf("Error usage: %s <graph> <n> <query> [-l] [-q]\n", argv[0]);
        printf("   -l => print labels to test/output/labels_out.txt\n");
        printf("   -q => print query results to test/output/queries_out.txt\n");
        return;
    }

    const char *graph_path = argv[1];
    const int num_intervals = atoi(argv[2]);
    const char *query_path = argv[3];
    const char gen_label_path[] = "test/output/labels_out.txt";
    const char res_query_path[] = "test/output/queries_out.txt";

    bool do_print_labels = false;
    bool do_print_queries = false;
    for(int i = 4; i < argc; i++) {
        if(strncmp(argv[i], "-l", 2) == 0)
            do_print_labels = true;
        else if(strncmp(argv[i], "-q", 2) == 0)
            do_print_queries = true;
        else {
            printf("%s is not a valid option\n", argv[i]);
            return;
        }
    };

    if(num_intervals <= 0) {
        printf("num_intervals must be >= 1\n");
        return;
    };

#if DEBUG
    printf("GRAPH PATH:             '%s'\n", graph_path);
    printf("TOT INTERVALS:          %d\n", num_intervals);
    printf("QUERY PATH:             '%s'\n", query_path);
    printf("PRINT LABELS TO FILE?   %s\n", do_print_labels == true ? "YES" : "NO");
    printf("PRINT QUERIES TO FILE?  %s\n", do_print_queries == true ? "YES" : "NO");
    printf("\n");
#endif 

    Graph* graph = graph_create(graph_path, num_intervals);
    if(graph == NULL) {
        fprintf(stderr, "FAILED graph_create at menu.c\n");
        exit(-1);
    }

#if 0 
    char graph_copy_path[] = "test/output/grafo.copy";
    printf("PRINTING GRAPH to '%s'\n", graph_copy_path);
    graph_print_to_stream(graph_copy_path, false, NULL, graph); 
    printf("DONE\n\n");
#endif

    label_generate_random_labels(graph);
    if(do_print_labels) {
        printf("PRINTING LABELS to '%s'\n", gen_label_path);
        label_print_to_file(gen_label_path, graph);
        printf("DONE\n\n");
    }

    query_init(query_path, graph);

    if(do_print_queries) {
        printf("PRINTING QUERIES RESULTS to '%s'\n", res_query_path);
        query_print_results(res_query_path);
        printf("DONE\n\n");
        cust_exit(graph);
    }
    else {
        while(true) { 
            int src, dst, query_number;
            bool err = false;
            printf("Enter query number: (-1 to finish)\n"); 
            int ret = scanf("%d", &query_number);
            if(ret != 1) {
                fprintf(stderr, "FAILED scanf of query_number at menu\n");
                err = true;
            }
            if(query_number == -1 || err == true) {
                cust_exit(graph);
                break;
            }
            int res = check_query(query_number, &src, &dst);
            switch(res) {
                case -1:
                    printf("The query number %d is out of boundary\n", query_number);
                    break;
                case -2:
                    printf("No results to be found. (tip: prob you didn't run query_init)\n");
                    exit(-6);
                    break;
                default:
                    printf(">> src %d dst %d is %s\n", src, dst, res == true ? "reachable" : "unreachable");
                    break;
            }
        }
    }

    return;
}
