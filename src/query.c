#include "query.h"
#include "labelling.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#define MAX_LENGTH_BUFFER 30 // max node_id = 2^32 - 1 is equivalent to 10 char of 1 byte. We multiple by 2 and rounded to 30. 
#define MAX_QUERIES 75000 
#define MULTITHREAD 1
#if MULTITHREAD
    #define MAX_THREADS 4
#endif
#define DEBUG 1 
/* fill the "queries" with the query read from the file at filepath
 * To access the ith query stored in "queries" use:
 *      queries[i*2] is the source node 
 *      queries[i*2 + 1] is the dest node
 * Return value: number of queries stored in "queries"
 */
static uint32_t query_read_from_file(char *filepath, uint32_t *queries);

static uint32_t *queries = NULL;
static uint32_t *thread_query_indeces = NULL; 
static Bitmap *query_results = NULL; 
static Graph *graph = NULL;
static sem_t sem; 
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
static bool *done_threads = NULL;

#if MULTITHREAD
static Bitmap* visited_nodes_multi[MAX_THREADS];
#else
static Bitmap *visited_nodes = NULL;
#if DEBUG
static is_first = true;
#endif
#endif

#if MULTITHREAD
static bool query_is_reachable_multi(uint32_t source_id, uint32_t dest_id, Graph* graph, Bitmap *vst_nodes);
static void *thread_starting_func(void *argument);
#endif


// TODO write to filepath
void query_to_file(char *filepath){
    uint32_t done = 0;
    FILE *fout = fopen("test/input/grafo_risultati.txt", "w");

    while(done < MAX_THREADS) {
        sem_wait(&sem);
        uint32_t start_index = -1;
        uint32_t end_index = -1;

        for(int i = 0; i < MAX_THREADS; i++) {
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
                fprintf(fout, "%d %d reachable\n", queries[i*2], queries[i*2+1]); 
            else
                fprintf(fout, "%d %d unreachable\n", queries[i*2], queries[i*2+1]); 
        }

        done++;
    
    }
    
}

void query_init(char *filepath, Graph *grafo){

   queries = malloc(2 * sizeof(uint32_t) * MAX_QUERIES); 
   if(queries == NULL){
       fprintf(stderr, "FAILED malloc at query_init\n");
       exit(1);
   }

   query_results = bitmap_create(MAX_QUERIES); 
   if(query_results == NULL){
       fprintf(stderr, "FAILED bitmap_create at query_init\n");
       exit(1);
   }

   done_threads = malloc(MAX_THREADS * sizeof(bool));
   if(done_threads == NULL){
       fprintf(stderr, "FAILED malloc of done_threads at query_init\n");
       exit(1);
   }

   for(uint32_t i = 0; i < MAX_THREADS; i++) {
       done_threads[i] = false;
   }

   graph = grafo;

#if DEBUG
   fprintf(stdout, "READING queries from %s...\n", filepath);
   clock_t start = clock();
#endif

   uint32_t num_tot_queries = query_read_from_file(filepath, queries);

#if DEBUG
   clock_t end = clock();
   printf("Read %d queries. It took %fs\n", num_tot_queries, (double)(end - start) / CLOCKS_PER_SEC);
#endif

#if MULTITHREAD
   for(uint32_t i = 0; i < MAX_THREADS; i++) {
       visited_nodes_multi[i] = bitmap_create(graph->num_nodes);
       if(visited_nodes_multi[i] == NULL) {
           fprintf(stderr, "FAILED bitmap_create at query_init for visited_nodes_multi\n");
           exit(2);
       }
   }
#endif

#if DEBUG
   fprintf(stdout, "SOLVING QUERIES ...\n"); 
   start = clock();
#endif

#if MULTITHREAD
   pthread_t thread_ids[MAX_THREADS];
   uint32_t start_index = 0;
   uint32_t num_queries_per_thread = num_tot_queries / MAX_THREADS;
   if(num_queries_per_thread == 0) {
       thread_query_indeces = malloc(2 * sizeof(uint32_t));
       thread_query_indeces[0] = start_index;
       thread_query_indeces[1] =  num_tot_queries;

       int err = pthread_create(&thread_ids[0], NULL, thread_starting_func, (void*) 0); 
       if(err != 0) {
           fprintf(stderr, "ERROR: pthread_create 0 with num_queries_per_thread == 0");
           exit(2);
       };

       pthread_join(thread_ids[0], NULL);
   }
   else {
       sem_init(&sem, 0, 0);
       thread_query_indeces = malloc(MAX_THREADS * 2 * sizeof(uint32_t));
       for(int j = 0; j < MAX_THREADS; j++) {
           thread_query_indeces[j*2] = -1;
           thread_query_indeces[j*2 + 1] = -1;
       }
#if DEBUG
       for(int j = 0; j < MAX_THREADS; j++) {
           fprintf(stdout, "thread_query_indeces %d %d %d\n", j, thread_query_indeces[j*2], thread_query_indeces[j*2+1]);
       }
#endif
       for(int i = 0; i < MAX_THREADS; i++) {
           thread_query_indeces[i*2] = start_index;
           thread_query_indeces[i*2 + 1] = i != (MAX_THREADS - 1) ? start_index + num_queries_per_thread : num_tot_queries;  
           start_index = thread_query_indeces[i*2 + 1];

           int err = pthread_create(&thread_ids[i], NULL, thread_starting_func, (void*) i); 
           if(err != 0) {
               fprintf(stderr, "ERROR: pthread_create %d", i);
               exit(2);
           };
       }
        
       pthread_t printer_id;
       int err = pthread_create(&printer_id, NULL, query_to_file, NULL); 
       if(err != 0) {
           fprintf(stderr, "ERROR: pthread_create printer\n");
           exit(2);
       };
        
       //waiting threads that are solving the queries
       for(int i = 0; i < MAX_THREADS; i++) {
           pthread_join(thread_ids[i], NULL);
       }
        
       //waiting thread that is printing the solutions
       pthread_join(printer_id, NULL);
       sem_destroy(&sem);

   }

   for(int i = 0; i < MAX_THREADS; i++) {
       bitmap_destroy(visited_nodes_multi[i]);
   }
#endif

#if DEBUG
   end = clock();
   fprintf(stdout, "SOLVING QUERIES took %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
#endif

};

static void *thread_starting_func(void *argument) {

    uint32_t id = (uint32_t) argument;
    uint32_t start_index = thread_query_indeces[id*2];
    uint32_t end_index = thread_query_indeces[id*2 + 1];;

#if DEBUG
    fprintf(stdout, "THREAD %lu> start_index %d end_index %d id %d\n", pthread_self(), start_index, end_index, id);
#endif

    for(uint32_t i = start_index; i < end_index; i++) {
        bitmap_clear_all(visited_nodes_multi[id]); 
        if(query_is_reachable_multi(queries[i*2], queries[i*2 + 1], graph, visited_nodes_multi[id])) {
            bitmap_set_bit(query_results, i);
        }
    }

    pthread_mutex_lock(&mutex);
    done_threads[id] = true;
    pthread_mutex_unlock(&mutex);
    sem_post(&sem);

}

static uint32_t query_read_from_file(char *filepath, uint32_t *queries){

    FILE* fp = fopen(filepath, "r");
    if(fp == NULL) {
        fprintf(stderr, "FAILED fopen at query_read_from_file");
        exit(1);
    }

    char buff[MAX_LENGTH_BUFFER];
    size_t next = 0;

#if DEBUG
    fprintf(stdout, "STARTING READING from file %s\n", filepath);
#endif

    while(fgets(buff, MAX_LENGTH_BUFFER, fp)){
        if(sscanf(buff, "%d %d", &queries[next * 2], &queries[next * 2 + 1]) == 2){  
#if DEBUG
            //fprintf(stdout, "Query: %u -> %d %d\n", next + 1, queries[next * 2], queries[next * 2 + 1]);
#endif
            next++;
        }
        else{
            fprintf(stderr, "FAILED sscanf at query_read_from_file\n");
            break;
        }
    }

#if DEBUG
    fprintf(stdout, "FINISHED READING from file %s\n", filepath);
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

static bool query_is_reachable_multi(uint32_t source_id, uint32_t dest_id, Graph* graph, Bitmap *vst_nodes){

#if DEBUG
    //fprintf(stdout, "src %d -> dest %d\n", source_id, dest_id);
#endif

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

#if 0 
    if(is_first) {
        fprintf(stdout, "recursing %d %d\n", source_id, dest_id);   
        is_first = true;
    }
#endif

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
            if(possible_path && query_is_reachable_multi(curr_child_id, dest_id, graph, vst_nodes)) {
                return true;
            }
        }
    }

    return false;

}

#if !MULTITHREAD
 /* check if node with id "source_id" can reach node with id "dest_id"
  * Return value: true if there is path from "source_id" to "dest_id", otherwise false;
  */
bool query_is_reachable(uint32_t source_id, uint32_t dest_id, Graph* graph){

    if(source_id == dest_id)
        return true;

    bitmap_set_bit(visited_nodes, source_id);

    Node* src_node = graph->nodes[source_id];
    Node* dst_node = graph->nodes[dest_id];
    uint32_t num_children_src = src_node->num_children;

    if(!is_a_possible_path(src_node, dst_node))
        return false;

#if DEBUG
    if(is_first) {
        fprintf(stdout, "recursing %d %d\n", source_id, dest_id);   
        is_first = true;
    }
#endif

    // check if it's a false positive
    for(int i = 0; i < num_children_src; i++) {
        uint32_t cur_child_id = src_node->children[i]; 
        // recurse only if child has not been visited  
        if(!bitmap_test_bit(visited_nodes, cur_child_id)){ 
            Node* cur_child_node = graph->nodes[cur_child_id];
            // if a interval of "dst_node" is not contained in the corresponding interval of "cur_child_node" 
            // then there is not a path from "cur_child_node" to "dst_node".
            bool possible_path = true;
            if(!is_a_possible_path(cur_child_node, dst_node))
                possible_path = false;
            
            // recurse if this child leads to a possiple path to "dst_node"
            if(possible_path && query_is_reachable(cur_child_id, dest_id, graph)) {
                return true;
            }
        }
    }

    return false;
}
#endif
