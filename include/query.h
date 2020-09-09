#ifndef QUERY_H
#define QUERY_H
#include <stdbool.h>
#include "graph.h"

#if TEST
/* check if there is a path from "source_id" to "dest_id"
 */
bool find_path_reachability(uint32_t source_id, uint32_t dest_id, Graph *graph, Bitmap *vst_nodes);
#endif

/* check the rechability of the queries stored in the file at "filepath". 
 * The file must be in this format:
 *    one query per row
 *    query format: "src_id dst_id"
 */
void query_init(const char *filepath, Graph *graph);
void query_cleanup(void);
void query_print_results(char *filepath);
bool check_query(int index, int *src, int *dst);

#endif
