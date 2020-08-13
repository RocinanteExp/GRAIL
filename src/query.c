#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include "query.h"
#include "label.h"
#include "constants.h"

#define MAX_LENGTH_BUFFER 30 // max node_id = 2^32 - 1 is equivalent to 10 char of 1 byte. We multiple by 2 and rounded to 30. 
#define MAX_QUERIES 75000 

typedef struct {
    uint32_t src;
    uint32_t dst;
} query;

typedef struct {
    uint32_t start;
    uint32_t end;
    Graph *graph;
#if DEBUG
    uint32_t id;
#endif
} thread_arg;

static uint32_t read_queries_from_file(char *filepath, query *queries);
static void *query_solver(void *argument);
static void *query_to_file(void *filepath);
bool find_path_reachability(uint32_t source_id, uint32_t dest_id, Graph* graph, Bitmap *vst_nodes);
void query_init(char *filepath, Graph *grafo);

static query *queries = NULL;
static Bitmap *query_results = NULL; 
static sem_t sem; 
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
static bool *done_threads = NULL;
static Bitmap* visited_nodes_multi[MAX_THREADS_QUERY];

/*
// TODO write to filepath
static void *query_to_file(void *path) {
    uint32_t done = 0;
    FILE *fout = fopen("test/input/grafo_risultati.txt", "w");

    while(done < MAX_THREADS_QUERY) {
        sem_wait(&sem);
        uint32_t start_index = -1;
        uint32_t end_index = -1;

        for(int i = 0; i < MAX_THREADS_QUERY; i++) {
            pthread_mutex_lock(&mutex);
            if(done_threads[i] == true) {
                pthread_mutex_unlock(&mutex);
                start_index = thread_query_indeces[i*2];
                end_index = thread_query_indeces[i*2 + 1];
                done_threads[i] = false;
                break;
            }
            else
                pthread_mutex_unlock(&mutex);
        }

#if DEBUG
        fprintf(stdout, "printing start %d end %d\n", start_index, end_index);
#endif
        for(int i = start_index; i < end_index; i++) {
            if(bitmap_test_bit(query_results, i))
                fprintf(fout, "%d %d 1\n", queries[i*2], queries[i*2+1]); 
            else
                fprintf(fout, "%d %d 0\n", queries[i*2], queries[i*2+1]); 
        }

        done++;
    }

    return query_to_file;
};
*/

static uint32_t compute_thread_query_indeces(uint32_t tot_queries, uint32_t max_threads, uint32_t *query_indeces) {

   const uint32_t num_queries_per_thread = tot_queries / max_threads;
   const uint32_t tot_running_threads = num_queries_per_thread == 0 ? 1 : MAX_THREADS_QUERY;
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

void query_init(char *filepath, Graph *g) {

   Graph *graph = g;
   queries = malloc(MAX_QUERIES * sizeof(query));
   if(queries == NULL){
       fprintf(stderr, "FAILED malloc'ing queries at query_init\n");
       exit(3);
   }

   query_results = bitmap_create(MAX_QUERIES); 
   if(query_results == NULL){
       fprintf(stderr, "FAILED bitmap_create at query_init\n");
       exit(4);
   }

   done_threads = malloc(MAX_THREADS_QUERY * sizeof(bool));
   if(done_threads == NULL){
       fprintf(stderr, "FAILED malloc of done_threads at query_init\n");
       exit(3);
   }

   for(uint32_t i = 0; i < MAX_THREADS_QUERY; i++) {
       done_threads[i] = false;
   }


#if DEBUG
   fprintf(stdout, "READING queries from %s...\n", filepath);
   clock_t start = clock();
#endif

   uint32_t num_tot_queries = read_queries_from_file(filepath, queries);

#if DEBUG
   clock_t end = clock();
   printf("Read %d queries. It took %fs\n", num_tot_queries, (double)(end - start) / CLOCKS_PER_SEC);
#endif

#if DEBUG
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
   sem_init(&sem, 0, 0);

   for(int i = 0; i < num_running_threads; i++) {
       args[i].start = thread_query_indeces[i*2];
       args[i].end = thread_query_indeces[i*2 + 1];
       args[i].graph = graph; 
#if DEBUG
       args[i].id = i;
#endif
       int err = pthread_create(&thread_ids[i], NULL, query_solver, &args[i]); 
       if(err != 0) {
           fprintf(stderr, "FAILED pthread_create for thread %d at query_init\n", i);  
           exit(6);
       };
   }
        
   pthread_t thread_printer_id;
   //int err = pthread_create(&thread_printer_id, NULL, query_to_file, NULL); 
   int err = pthread_create(&thread_printer_id, NULL, NULL, NULL); 
   if(err != 0) {
       fprintf(stderr, "FAILED pthread_create for thread printer ath query_init\n");
       exit(6);
   };
        
   //waiting threads that are solving the queries
   for(int i = 0; i < num_running_threads; i++) {
       pthread_join(thread_ids[i], NULL);
   }
   //waiting thread that is printing the solutions
   pthread_join(thread_printer_id, NULL);

   for(int i = 0; i < num_running_threads; i++) {
       bitmap_destroy(visited_nodes_multi[i]);
   }

   sem_destroy(&sem);

#if DEBUG
   end = clock();
   fprintf(stdout, "SOLVING QUERIES took %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
#endif

};

static void *query_solver(void *argument) {

    thread_arg *arg= (thread_arg *) argument;
    uint32_t start_index = arg->start; 
    uint32_t end_index = arg->end; 
    Graph *graph = arg->graph; 

#if DEBUG
    uint32_t id= arg->id; 
    fprintf(stdout, "THREAD %lu start_index %u end_index %u id %u\n", pthread_self(), start_index, end_index, id);
#endif

    for(uint32_t i = start_index; i < end_index; i++) {
        bitmap_clear_all(visited_nodes_multi[id]); 
        if(find_path_reachability(queries[i].src, queries[i].dst, graph, visited_nodes_multi[id])) {
            bitmap_set_bit(query_results, i);
        }
    }

    pthread_mutex_lock(&mutex);
    done_threads[id] = true;
    pthread_mutex_unlock(&mutex);
    sem_post(&sem);
    pthread_exit(NULL);

};

static uint32_t read_queries_from_file(char *filepath, query *queries) {

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
        if(sscanf(buff, "%d %d", &queries[next].src, &queries[next].dst) == 2){  
#if 1 
            fprintf(stdout, "%d %d", queries[next].src, queries[next].dst);
#endif
            next++;
        }
        else{
            fprintf(stderr, "FAILED sscanf at read_queries_from_file\n");
            break;
        }
    }

#if DEBUG
    fprintf(stdout, "Read %d queries.\nExiting read_queries_from_file\n", next);
#endif

    return next;
}

// check is there is a possible path from source to dest by checking that all label intervals of dest
// are contained inside the corresponding interval of source
static bool is_a_possible_path(Node* source, Node* dest) {
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
bool find_path_reachability(uint32_t source_id, uint32_t dest_id, Graph* graph, Bitmap *vst_nodes){

    if(source_id == dest_id)
        return true;

    bitmap_set_bit(vst_nodes, source_id);

    Node* src_node = graph->nodes[source_id];
    if(src_node == NULL) {
        fprintf(stdout, "NULLO %d\n", source_id);
        exit(1);
    }

    Node* dst_node = graph->nodes[dest_id];
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
            Node* cur_child_node = graph->nodes[curr_child_id];
            // if a interval of "dst_node" is not contained in the corresponding interval of "cur_child_node" 
            // then there is not a path from "cur_child_node" to "dst_node".
            bool possible_path = true;
            if(!is_a_possible_path(cur_child_node, dst_node))
                possible_path = false;
            
            // recurse if this child leads to a possiple path to "dst_node"
            if(possible_path && find_path_reachability(curr_child_id, dest_id, graph, vst_nodes)) {
                return true;
            }
        }
    }

    return false;

}
