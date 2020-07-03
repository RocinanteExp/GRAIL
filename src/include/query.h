#ifndef QUERY_H
#define QUERY_H
#include <stdbool.h>
#include "graph.h"

/* check is if the node with id "source_id" can reach the node with id "dest_id"
 */
bool query_is_reachable(int32_t source_id, int32_t dest_id, Graph* graph);
/* check the rechability of the queries stored in the file at filepath. 
 * The file format is:
 *    one query per row
 *    query format: "src dst"
 */
void query_init(char *filepath, Graph *graph);

#endif

