#include "menu.h"
#include "graph.h"
#include "label.h"
#include "query.h"
#include "constants.h"

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
    printf("Graph path:       %s\n", graph_path);
    printf("Num of intervals: %d\n", num_intervals);
    printf("Query path:       %s\n", query_path);
    printf("Print labels to file  %s\n", do_print_labels == true ? "YES" : "NO");
    printf("Print queries to file %s\n", do_print_queries == true ? "YES" : "NO");
#endif 

    Graph* graph = graph_create(graph_path, num_intervals);
    if(graph == NULL) {
        fprintf(stderr, "something went wrong :\\\n");
        exit(-1);
    }
#if DEBUG
    char copy_path[100];
    int n = sprintf(copy_path, "test/output/graph.copy");
    if(n < 0) {
        fprintf(stderr, "FAILED sprintf at menu.c\n");
    }
    else
        graph_print_to_stream(copy_path, false, NULL, graph); 
#endif
    label_generate_random_labels(graph);
    if(do_print_labels)
        label_print_to_file("test/output/labels_out.txt", graph);

    query_init(query_path, graph);

    if(do_print_queries)
        query_print_results("test/output/queries_out.txt");
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
                printf("Thank you for using our program\n");
                query_cleanup();
                graph_destroy(graph);
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
