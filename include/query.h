#ifndef QUERY_H
#define QUERY_H
#include <stdbool.h>
#include "graph.h"

/* check is if the node with id "source_id" can reach the node with id "dest_id"
 */
bool query_is_reachable_multi(uint32_t source_id, uint32_t dest_id, Graph* graph, Bitmap *vst_nodes);
/* check the rechability of the queries stored in the file at filepath. 
 * The file format is:
 *    one query per row
 *    query format: "src dst"
 */
void query_init(char *filepath, Graph *graph);

#endif
