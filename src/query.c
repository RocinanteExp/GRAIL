#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include "query.h"
#include "label.h"
#include "constants.h"

#define MAX_LENGTH_BUFFER 30 // max node_id = 2^32 - 1 is equivalent to 10 char of 1 byte. We multiple by 2 and rounded to 30. 

typedef struct {
    uint32_t src;
    uint32_t dst;
} route;

typedef struct {
    route *routes;
    Bitmap *res;
    uint32_t length;
} query;

typedef struct {
    uint32_t start;
    uint32_t end;
    Graph *graph;
    uint32_t id;
} thread_arg;

static uint32_t read_queries_from_file(const char *filepath, query *queries);
static void *query_solver(void *argument);
static void query_print_results_to_file(query *queries, uint32_t length, char *filepath);
static query* init_query_struct(uint32_t size);
static uint32_t compute_thread_query_indeces(uint32_t tot_queries, uint32_t max_threads, uint32_t *query_indeces);
static void destroy_query_struct(query *q);

#if !TEST
static 
#endif
bool find_path_reachability(uint32_t source_id, uint32_t dest_id, Graph* graph, Bitmap *vst_nodes);

static query *queries = NULL;
static Bitmap* visited_nodes_multi[MAX_THREADS_QUERY] = {NULL};

void query_print_results(char *filepath) {
    query_print_results_to_file(queries, queries->length, filepath); 
}

void query_cleanup(void) {
    destroy_query_struct(queries);
}

bool check_query(int index, int *src, int *dst) {
    *src = queries->routes[index].src;
    *dst = queries->routes[index].dst;
    if(bitmap_test_bit(queries->res, index))
        return true;
    return false;
}

void query_init(const char *filepath, Graph *g) {

   Graph *graph = g;
   queries = init_query_struct(MAX_QUERIES);

#if DEBUG
   fprintf(stdout, "READING queries from '%s'...\n", filepath);
   clock_t start = clock();
#endif

   uint32_t num_tot_queries = read_queries_from_file(filepath, queries);
   queries->length = num_tot_queries;

#if DEBUG
   clock_t end = clock();
   printf("READ %d queries. It took %fs\n\n", num_tot_queries, (double)(end - start) / CLOCKS_PER_SEC);
   fprintf(stdout, "STARTING SOLVING QUERIES ...\n"); 
   start = clock();
#endif

   pthread_t thread_ids[MAX_THREADS_QUERY];
   uint32_t thread_query_indeces[MAX_THREADS_QUERY * 2];
   const uint32_t num_running_threads = compute_thread_query_indeces(num_tot_queries, MAX_THREADS_QUERY, thread_query_indeces);

#if DEBUG
   for(int j = 0; j < num_running_threads; j++) {
       fprintf(stdout, "thread_query_indeces %d %d %d\n", j, thread_query_indeces[j*2], thread_query_indeces[j*2+1]);
   }
#endif

   for(uint32_t i = 0; i < num_running_threads; i++) {
       visited_nodes_multi[i] = bitmap_create(graph->num_nodes);
       if(visited_nodes_multi[i] == NULL) {
           fprintf(stderr, "FAILED bitmap_create at query_init for visited_nodes_multi\n");
           exit(3);
       }
   }

   thread_arg args[num_running_threads];

   for(int i = 0; i < num_running_threads; i++) {
       args[i].start = thread_query_indeces[i*2];
       args[i].end = thread_query_indeces[i*2 + 1];
       args[i].graph = graph; 
       args[i].id = i;

       int err = pthread_create(&thread_ids[i], NULL, query_solver, &args[i]); 
       if(err != 0) {
           fprintf(stderr, "FAILED pthread_create for thread %d at query_init\n", i);  
           exit(6);
       };
   }
        
   //waiting threads... 
   for(int i = 0; i < num_running_threads; i++) {
       pthread_join(thread_ids[i], NULL);
   }

   for(int i = 0; i < num_running_threads; i++) {
       bitmap_destroy(visited_nodes_multi[i]);
   }

#if DEBUG
   end = clock();
   fprintf(stdout, "SOLVING %u QUERIES took %fs\n", queries->length, (double)(end - start) / CLOCKS_PER_SEC);
#endif

};

static void *query_solver(void *argument) {

    thread_arg *arg = (thread_arg *) argument;
    uint32_t start_index = arg->start; 
    uint32_t end_index = arg->end; 
    Graph *graph = arg->graph; 
    uint32_t id = arg->id; 

#if DEBUG
    fprintf(stdout, "THREAD %u start_index %u end_index %u graph %p visited_notes_multi %p\n", id, start_index, end_index, graph, visited_nodes_multi[id]);
#endif

    for(uint32_t i = start_index; i < end_index; i++) {
        bitmap_clear_all(visited_nodes_multi[id]); 
        if(find_path_reachability(queries->routes[i].src, queries->routes[i].dst, graph, visited_nodes_multi[id])) {
            bitmap_set_bit(queries->res, i);
        }
    }

    pthread_exit(NULL);
};

static uint32_t read_queries_from_file(const char *filepath, query *queries) {

#if DEBUG
    fprintf(stdout, "> starting reading queries from file %s\n", filepath);
#endif

    FILE *fp = fopen(filepath, "r");
    if(fp == NULL) {
        fprintf(stderr, "FAILED fopen at read_queries_from_file");
        exit(5);
    }

    char buff[MAX_LENGTH_BUFFER];
    size_t next = 0;


    while(fgets(buff, MAX_LENGTH_BUFFER, fp)){
        if(sscanf(buff, "%d %d", &queries->routes[next].src, &queries->routes[next].dst) == 2){  
#if 0 
            fprintf(stdout, "%d %d\n", queries->routes[next].src, queries->routes[next].dst);
#endif
            next++;
        }
        else{
            fprintf(stderr, "FAILED sscanf at read_queries_from_file\n");
            break;
        }
    }

#if DEBUG
    fprintf(stdout, "> Read %ld queries.\n> Exiting read_queries_from_file\n", next);
#endif

    fclose(fp);
    return next;
}

// check is there is a possible path from source to dest by checking that all label intervals of dest
// are contained inside the corresponding interval of source
static bool is_a_possible_path(Node *source, Node *dest) {
    uint32_t num_intervals = source->num_intervals;
    for(uint32_t i = 0; i < num_intervals; i++){
        if(!label_include(dest->intervals[i], source->intervals[i])){
            return false;
        }
    }
    return true;
}

#if !TEST
static 
#endif
bool find_path_reachability(uint32_t source_id, uint32_t dest_id, Graph *graph, Bitmap *vst_nodes){

    //printf("source_id %u dest_id %u\n", source_id, dest_id);

    if(source_id == dest_id)
        return true;

    bitmap_set_bit(vst_nodes, source_id);

    Node *src_node = graph->nodes[source_id];
    if(src_node == NULL) {
        fprintf(stdout, "NULLO %d\n", source_id);
        exit(1);
    }

    Node *dst_node = graph->nodes[dest_id];
    if(dst_node == NULL) {
        fprintf(stdout, "NULLO %d\n", dest_id);
        exit(1);
    }
    uint32_t num_children_src = src_node->num_children;

    if(!is_a_possible_path(src_node, dst_node))
        return false;

    // check if it's a false positive
    for(int i = 0; i < num_children_src; i++) {
        uint32_t curr_child_id = src_node->children[i]; 
        // recurse only if child has not been visited  
        if(!bitmap_test_bit(vst_nodes, curr_child_id)){ 
            Node* curr_child_node = graph->nodes[curr_child_id];
            // if a interval of "dst_node" is not contained in the corresponding interval of "curr_child_node" 
            // then there is not a path from "curr_child_node" to "dst_node".
            bool possible_path = true;
            if(!is_a_possible_path(curr_child_node, dst_node))
                possible_path = false;
            
            // recurse if this child leads to a possiple path to "dst_node"
            if(possible_path && find_path_reachability(curr_child_id, dest_id, graph, vst_nodes)){
                return true;
            }
        }
    }

    return false;

}

static query* init_query_struct(uint32_t size) {
    query *q = malloc(sizeof(query));
    q->routes = malloc(size * sizeof(route));
    q->res = bitmap_create(size);
    q->length = size;
    return q;
}

static void destroy_query_struct(query *q) {
    bitmap_destroy(q->res);
    free(q->routes);
    free(q);
}

static void query_print_results_to_file(query *queries, uint32_t length, char *filepath) {

    if(filepath == NULL)
        filepath = "../test/output/query_output.txt"; 

    FILE *fout = fopen(filepath, "w");
    if(fout == NULL) {
        fprintf(stderr, "FAILED opening file %s at query_print_results_to_file\n", filepath);
        exit(4);
    }

    for(int i = 0; i < length; i++) {
        if(bitmap_test_bit(queries->res, i))
            fprintf(fout, "%d %d 1\n", queries->routes[i].src, queries->routes[i].dst); 
        else
            fprintf(fout, "%d %d 0\n", queries->routes[i].src, queries->routes[i].dst); 
    }

};

static uint32_t compute_thread_query_indeces(uint32_t tot_queries, uint32_t max_threads, uint32_t *query_indeces) {

   const uint32_t num_queries_per_thread = tot_queries / max_threads;
   const uint32_t tot_running_threads = num_queries_per_thread == 0 ? 1 : max_threads;
   uint32_t start_index = 0;

   for(int i = 0; i < tot_running_threads; i++) {
       query_indeces[i*2] = start_index;
       if(i != MAX_THREADS_QUERY - 1)
           query_indeces[i*2 + 1] = start_index + num_queries_per_thread;
       else
           query_indeces[i*2 + 1] = tot_queries; 

       start_index = query_indeces[i*2 + 1];
   }

   return tot_running_threads;
}
