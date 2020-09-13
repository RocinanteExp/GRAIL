#ifndef QUERY_H
#define QUERY_H
#include <stdbool.h>
#include "graph.h"

#if TEST
/* check if there is a path from "source_id" to "dest_id"
 */
bool find_path_reachability(uint32_t source_id, uint32_t dest_id, Graph *graph, uint32_t *vst_nodes, uint32_t q_ind);
#endif

/* check the rechability of the queries stored in the file at "filepath". 
 * The file must be in this format:
 *    one query per row
 *    query format: "src_id dst_id"
 */
void query_init(const char *filepath, Graph *graph);
void query_cleanup(void);
/* Prints the results of the query to file at filepath 
 * Return: 
 *    on error -1
 */
int query_print_results(const char *filepath);

/* Returns the result of the query at position "index", i.e. returns the value found by query_init for the query at line num "index". Furthermore, it going to save at *src and *dst respectively the source node id and the dest node id.
 * Return: 
 *    -1 in case of index is out of boundary 
 *    0 in case there is not a path from src to dst
 *    1 in case there is a path from src to dst
 */
int check_query(int index, int *src, int *dst);

#endif
