#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include "query.h"
#include "label.h"
#include "constants.h"
#include "time_tracker.h" 

#define MAX_LENGTH_BUFFER 30 // max node_id = 2^32 - 1 is equivalent to 10 char of 1 byte. We multiple by 2 and rounded to 30. 

typedef struct {
    uint32_t src;
    uint32_t dst;
} route;

typedef struct {
    route *routes;
    Bitmap *res;
    uint32_t length;
} query_set;

typedef struct {
    uint32_t start;
    uint32_t end;
    Graph *graph;
    uint32_t id;
} thread_arg;

static uint32_t read_queries_from_file(const char *filepath, query_set *queries);
static uint32_t find_max_queries(const char *filepath);
static uint32_t compute_thread_query_indeces(uint32_t tot_queries, uint32_t max_threads, uint32_t *query_indeces);
static void *query_solver(void *argument);
static query_set* init_query_struct(uint32_t size);
static void destroy_query_struct(query_set *q);
static void query_print_results_to_file(query_set *queries, uint32_t length, char *filepath);

#if !TEST
static 
#endif
bool find_path_reachability(uint32_t source_id, uint32_t dest_id, Graph* graph, Bitmap *vst_nodes);

static query_set *queries = NULL;
static Bitmap *visited_nodes_multi[MAX_THREADS_QUERY] = {NULL};

int query_print_results(char *filepath) {
    if(queries == NULL)
        return -1;
    query_print_results_to_file(queries, queries->length, filepath); 
    return 0;
}

void query_cleanup(void) {
    destroy_query_struct(queries);
}

int check_query(int index, int *src, int *dst) {

    if(queries == NULL) {
        fprintf(stderr, "You have to run query_init before of check_query\n");
        return -2;
    }

    if(index < 0 || index >= queries->length)
        return -1;
    *src = queries->routes[index].src;
    *dst = queries->routes[index].dst;
    if(bitmap_test_bit(queries->res, index))
        return 1;
    return 0;

}

void query_init(const char *filepath, Graph *g) {

   Graph *graph = g;
   uint32_t num_max_queries = find_max_queries(filepath);
   queries = init_query_struct(num_max_queries);

#if DEBUG
   long before_time, after_time;
   before_time = get_now();
   fprintf(stdout, "READING QUERIES from '%s'...\n", filepath);
#endif

   uint32_t num_tot_queries = read_queries_from_file(filepath, queries);
   queries->length = num_tot_queries;

#if DEBUG
   after_time = get_now();
   fprintf(stdout, "READ %d queries. It took %ld ms\n\n", num_tot_queries, after_time - before_time);

   before_time = get_now();
   fprintf(stdout, "SOLVING QUERIES ...\n"); 
#endif

   pthread_t thread_ids[MAX_THREADS_QUERY];
   uint32_t thread_query_indeces[MAX_THREADS_QUERY * 2];
   // num_running_threads is equal to MAX_THREADS_QUREY in most cases
   const uint32_t num_running_threads = compute_thread_query_indeces(num_tot_queries, MAX_THREADS_QUERY, thread_query_indeces);

   for(uint32_t i = 0; i < num_running_threads; i++) {
       visited_nodes_multi[i] = bitmap_create(graph->num_nodes);
       if(visited_nodes_multi[i] == NULL) {
           fprintf(stderr, "FAILED bitmap_create for visited_nodes_multi at query_init\n");
           exit(-4);
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
           exit(-3);
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
   after_time = get_now();
   fprintf(stdout, "SOLVING %u QUERIES took %ld ms\n", queries->length, after_time - before_time);
#endif

};

static void *query_solver(void *argument) {

    thread_arg *arg = (thread_arg *) argument;
    uint32_t start_index = arg->start; 
    uint32_t end_index = arg->end; 
    Graph *graph = arg->graph; 
    uint32_t id = arg->id; 

#if DEBUG
    fprintf(stdout, ">> THREAD %u start_index %u end_index %u\n", id, start_index, end_index);
#endif

    for(uint32_t i = start_index; i < end_index; i++) {
        bitmap_clear_all(visited_nodes_multi[id]); 
        if(find_path_reachability(queries->routes[i].src, queries->routes[i].dst, graph, visited_nodes_multi[id])) {
            bitmap_set_bit(queries->res, i);
        }
    }

    pthread_exit(NULL);
};

static uint32_t find_max_queries(const char *filepath) {

    FILE *fp = fopen(filepath, "r");
    if(fp == NULL) {
        fprintf(stderr, "FAILED fopen at find_max_queries of %s\n", filepath);
        exit(-4);
    }

    char buff[MAX_LENGTH_BUFFER];
    size_t lines = 0;

    while(fgets(buff, MAX_LENGTH_BUFFER, fp))
        lines++;

    fclose(fp);
    return lines;
}

static uint32_t read_queries_from_file(const char *filepath, query_set *queries) {

    FILE *fp = fopen(filepath, "r");
    if(fp == NULL) {
        fprintf(stderr, "FAILED fopen at read_queries_from_file");
        exit(-4);
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

    if(source_id == dest_id)
        return true;

    bitmap_set_bit(vst_nodes, source_id);

    Node *src_node = graph->nodes[source_id];
    if(src_node == NULL) {
        fprintf(stdout, "Node src %u was not found in this graph at find_path_reachability\n", source_id);
        exit(-5);
    }

    Node *dst_node = graph->nodes[dest_id];
    if(dst_node == NULL) {
        fprintf(stdout, "Node dst %u was not found in this graph at find_path_reachability\n", dest_id);
        exit(-5);
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

static query_set *init_query_struct(uint32_t size) {
    query_set *q = malloc(sizeof(query_set));
    q->routes = malloc(size * sizeof(route));
    q->res = bitmap_create(size);
    q->length = size;
    return q;
}

static void destroy_query_struct(query_set *q) {
    bitmap_destroy(q->res);
    free(q->routes);
    free(q);
}

static void query_print_results_to_file(query_set *queries, uint32_t length, char *filepath) {

    if(filepath == NULL)
        filepath = "test/output/query_output.txt"; 

    FILE *fout = fopen(filepath, "w");
    if(fout == NULL) {
        fprintf(stderr, "FAILED fopen of %s in write mode at query_print_results_to_file\n", filepath);
        exit(-3);
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
