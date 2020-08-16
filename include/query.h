#ifndef QUERY_H
#define QUERY_H
#include <stdbool.h>
#include "graph.h"

#if TEST
/* check is if the node with id "source_id" can reach the node with id "dest_id"
 */
bool find_path_reachability(uint32_t source_id, uint32_t dest_id, Graph *graph, Bitmap *vst_nodes);
/* check the rechability of the queries stored in the file at filepath. 
 * The file format is:
 *    one query per row
 *    query format: "src dst"
 */
#endif

void query_init(const char *filepath, Graph *graph);
void query_cleanup(void);
void query_print_results(char *filepath);

#endif
