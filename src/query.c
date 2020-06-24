#include "query.h"
#include "Labeling.h"
#include <stdlib.h>
#include <stdio.h>
#define MAX_LENGTH_BUFFER 4096 
#define MAX_LENGTH_QUERY 8192 
#define DEBUG 1 

/* fill the "queries" with the query read from the file at filepath
 * To access the ith query stored in "queries" use:
 *      queries[i*2] is the source node 
 *      queries[i*2 + 1] is the dest node
 * Return value: number of queries stored in "queries"
 */
static uint32_t query_read_from_file(char *filepath, int32_t *queries);
// for now this function is not currently used 
static bool dfs(int32_t source_id, int32_t dest_id, Graph* graph);
// used for tracking the visited nodes in the dfs. It is used in "query_is_reachable" 
static Bitmap* visited_nodes = NULL;

void query_init(char *filepath, Graph *graph){

   int *queries = malloc(sizeof(int) * MAX_LENGTH_QUERY * 2); 
   if(queries == NULL){
       fprintf(stderr, "ERROR function query_init: failed malloc for query");
       exit(1);
   }

   visited_nodes = bitmap_create(graph->num_nodes);
   uint32_t num_tot_queries = query_read_from_file(filepath, queries);
   for(int i = 0; i < num_tot_queries; i++){
       bitmap_clear_all(visited_nodes); 
       if(query_is_reachable(queries[i*2], queries[i*2 + 1], graph))
               fprintf(stdout, "Query %2d:  SRC %2d -> DST %2d REACHABLE\n", i+1, queries[i*2], queries[i*2 + 1]);
       else
               fprintf(stdout, "Query %2d:  SRC %2d -> DST %2d UNREACHABLE\n", i+1, queries[i*2], queries[i*2 + 1]);
   }

}

static uint32_t query_read_from_file(char *filepath, int32_t *queries){

   FILE* fp = fopen(filepath, "rb");
   if(fp == NULL){
       fprintf(stderr, "ERROR function query_init: failed fopen ");
   }

   char *buff = malloc(sizeof(char) * MAX_LENGTH_BUFFER); 
   if(buff == NULL){
       fprintf(stderr, "ERROR function query_init: failed malloc for buff");
       exit(1);
   }

   size_t next = 0;
   while(fgets(buff, MAX_LENGTH_BUFFER, fp)){
       if(sscanf(buff, "%d %d", &queries[next * 2], &queries[next * 2 + 1]) == 2){  
#if DEBUG
           fprintf(stdout, "next: %I64u -> %d %d\n", next + 1, queries[next * 2], queries[next * 2 + 1]);
#endif
           next++;
       }
       else{
           fprintf(stderr, "ERROR function query_init: failed sscanf");
           break;
       }
   }

   return next;

}

 /* check if node with id "source_id" can reach node with id "dest_id"
  * Return value: true if there is path from "source_id" to "dest_id", otherwise false;
  */
bool query_is_reachable(int32_t source_id, int32_t dest_id, Graph* graph){


    if(source_id == dest_id)
        return true;

    Node* src_node = graph->nodes[source_id];
    Node* dst_node = graph->nodes[dest_id];
    uint32_t num_intervals = graph->num_intervals;
    uint32_t num_children_src = src_node->num_children;

    // check if the ith interval label of "dest_id" is contained in the ith interval label of "source_id"
    for(int i = 0; i < num_intervals; i++){
        if(!label_include(dst_node->intervals[i], src_node->intervals[i])){
            return false;
        }
    }

    //if(is_reachable && !dfs(source_id, dest_id, graph)){
    //    is_reachable = false; 
    //}

    // all interval labels of "dest_id" are contained in the interval labels of "source_id" 
    // check if it's a false positive by performing a dfs 
    for(int i = 0; i < num_children_src; i++){
        uint32_t cur_child_id = src_node->children[i]; 
        // recurse only if child has not been visited  
        if(!bitmap_test_bit(visited_nodes, cur_child_id)){ 
            Node* cur_child_node = graph->nodes[cur_child_id];
            
            // if a interval of "dst_node" is not contained in the corresponding interval of "cur_child_node" 
            // then there is not a path from "cur_child_node" to "dst_node".
            bool possible_path = true;
            for(int j = 0; j < num_intervals; j++){
                if(!label_include(dst_node->intervals[j], cur_child_node->intervals[j])){
                    possible_path = false;
                    break;
                }
            }
            
            // recurse if this child leads to a possiple path to "dst_node"
            if(possible_path && query_is_reachable(cur_child_id, dest_id, graph)){
                return true;
            }

            bitmap_set_bit(visited_nodes, cur_child_id);

        }

    }

    return false;

}

// for now this function is not used
static bool dfs(int32_t source_id, int32_t dest_id, Graph* graph){

    if(source_id == dest_id)
        return true;
    
    Node* cur_node = graph->nodes[source_id];
    uint32_t num_children = cur_node->num_children;
    for(int i = 0; i < num_children; i++){
       if(!bitmap_test_bit(visited_nodes, cur_node->children[i]))
               if(dfs(cur_node->children[i], dest_id, graph))
                   return true;
    }

    return false;

}

/*int main(int argc, char **argv){
    
    Graph* graph = graph_create("grafo.gra", 1);
    graph_print(graph, true, -1); 
    graph_randomize_labelling_sequential(graph, 1);
    graph_print(graph, true, -1); 
    query_init("queries.que", graph); 

}*/
