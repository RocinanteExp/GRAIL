#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "graph.h"
#define MAGIC_NUMBER 4096 
#define BUFFER_SIZE 10240 

static void* thread_entry_point(void *thread_argument);
static void node_add_children(Node *node, const char *str);

struct thread_arg {
    FILE *fin;
    Graph *graph;
    pthread_spinlock_t *lock;
    Bitmap *inc_edge_nodes;
    uint32_t *curr_iteration;
    uint32_t num_intervals;
    uint32_t tot_nodes;
};

// save the starting node of each multi node 
static Bitmap *base_multi_nodes = NULL;
static const uint32_t max_nodes_per_thread = 100000;

Node* node_create_multiple(uint32_t num_intervals, uint32_t* node_ids, uint32_t num_nodes)
{
    Node* base_node = malloc(num_nodes * sizeof(Node));
    if (base_node == NULL) {
        fprintf(stderr, "FAILED malloc of base_node at node_create_multiple.\n%d a node_id", node_ids[0]);
        return NULL;
    }

    Label* base_interval = malloc(num_nodes * num_intervals * sizeof(Label));
    if (base_interval == NULL) {
        fprintf(stderr, "FAILED malloc of base_interval at node_create_multiple.\n%d a node_id", node_ids[0]);
        return NULL;
    }

    for (int i = 0; i < num_nodes ; i++) {
        Node* curr_node = base_node + i;
        curr_node->intervals = base_interval + i*num_intervals;

        for(int j = 0; j < num_intervals; j++) {
            curr_node->intervals[j].left = UINT32_MAX;
            curr_node->intervals[j].right = UINT32_MAX;
        }

        curr_node->id = node_ids[i];
        curr_node->children = NULL;
        curr_node->num_children = 0;
        curr_node->num_intervals = num_intervals;
    }

    return base_node;
}

void node_destroy_multiple(Graph *graph)
{
    
    for(uint32_t i = 0; i < graph->num_nodes; i++)
    {
        Node *curr_node = graph->nodes[i];
        free(curr_node->children);
    }

    for(uint32_t i = 0; i < graph->num_nodes; i++)
    {
        if(bitmap_test_bit(base_multi_nodes, i) == 1)
        {
            Node *curr_node = graph->nodes[i];
            free(curr_node->intervals);
            free(curr_node);
        }
    }
}

static void node_add_children(Node* node, const char* str)
{
    uint32_t str_length = strlen(str);
    int begin_index = 0;
    uint32_t numbers[4096] = {0};
    uint32_t next = 0;
    for(int i = 0; i < str_length; i++) {
        if(str[i] >= '0' && str[i] <= '9') { // 48 == '0'; 57 == '9'
            if(begin_index == -1) {
                begin_index = i;
            }
        }
        else if (str[i] == ' ' || str[i] == ':') { // 32 == space; 58 == :
            if(begin_index != -1) {
                numbers[next] = atoi(str + begin_index);
                next++;
                if(next >= 4096) {
                fprintf(stdout, "Line is too long:\n%s", str);
                    fprintf(stdout, "Exiting...\n");
                    exit(3);
                }
                begin_index = -1;
            }
        }
        else if (str[i] == '#' || str[i] == 13 || str[i] == 10) { // 35 = #; 13 = carriage return; 10 = new line feed
            break;
        }
        else {
            fprintf(stdout, "UNKNOWN CHARACTER %d\n", str[i]);
            fprintf(stdout, "Exiting...");
            exit(3);
        }
    }

    next = next - 1; // skip the node id
    node->children = malloc(next * sizeof(uint32_t));
    if(node->children == NULL) {
        fprintf(stderr, "failed malloc on node_add_children\n");
        exit(1);
    }
    memcpy(node->children, &numbers[1], next * sizeof(uint32_t));
    node->num_children = next;
}

int find_root_nodes(Graph* p_graph, Bitmap* b_incoming_edge_nodes)
{
    uint32_t next = 0;
    uint32_t magic_number = MAGIC_NUMBER;
    for(int i = 0; i < p_graph->num_nodes; i++) {
        if(!bitmap_test_bit(b_incoming_edge_nodes, i)) {
            p_graph->root_nodes[next++] = i;
        }
        if(next >= magic_number) {
            magic_number *= 2;
            p_graph->root_nodes = realloc(p_graph->root_nodes, magic_number * sizeof(uint32_t));
            if(p_graph->root_nodes == NULL){
                free(p_graph->nodes);
                free(p_graph);
                fprintf(stderr, "FAILED realloc graph->root_nodes at find_root_nodes\n");
                return -1;
            }
        }
    }

    return next;
}

Graph *graph_create(const char *filepath, const int num_intervals)
{
#if DEBUG
    clock_t start = clock();
#endif

    pthread_spinlock_t s_lock;
    const uint16_t BUFF_SIZE = 8192;
    uint32_t curr_iteration = 0;
    uint32_t num_nodes = 0;

    FILE *fin = fopen( filepath, "r");
    if(fin == NULL){
        fprintf(stderr, "FAILED opening the file %s at graph_create\n", filepath);
        return NULL;
    }

    Graph *p_graph = malloc(sizeof(Graph));
    if(p_graph == NULL){
        fprintf(stderr, "FAILED malloc p_graph at graph_create\n");
        return NULL;
    }

    // parsing the first line
    char curr_line[BUFF_SIZE];
    fgets(curr_line, BUFF_SIZE, fin);
    sscanf(curr_line, "%u", &num_nodes);
#if DEBUG
    printf("Num of nodes %u\n", num_nodes);
#endif
    p_graph->num_nodes = num_nodes;
    Bitmap *b_incoming_edge_nodes = bitmap_create(num_nodes);

    p_graph->nodes = malloc(num_nodes * sizeof(Node*));
    if(p_graph->nodes == NULL)
    {
        free(p_graph);
        fprintf(stderr, "FAILED malloc p_graph->nodes at graph_create\n");
        return NULL;
    }

    base_multi_nodes = bitmap_create(num_nodes); 

    int err = pthread_spin_init(&s_lock, 0);
    if(err != 0){
        fprintf(stderr, "FAILED pthread_spin_init at graph_create\n");
        return NULL;
    };

    pthread_t thread_ids[MAX_THREADS_GRAPH];
    struct thread_arg thread_arg = {
        .fin = fin, .graph = p_graph, .num_intervals = num_intervals,
        .curr_iteration = &curr_iteration, .tot_nodes = num_nodes, .lock = &s_lock,
        .inc_edge_nodes = b_incoming_edge_nodes
    };

    for(int i = 0; i < MAX_THREADS_GRAPH; i++) {
        int err = pthread_create(&thread_ids[i], NULL, thread_entry_point, (void*) &thread_arg);
        if(err != 0) {
            fprintf(stderr, "FAILED creating %d thread", i);
            return NULL;
        };
    }

    for(int i = 0; i < MAX_THREADS_GRAPH; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    pthread_spin_destroy(&s_lock);

    p_graph->root_nodes = malloc(MAGIC_NUMBER * sizeof(uint32_t));
    if(p_graph->root_nodes == NULL){
        free(p_graph->nodes);
        free(p_graph);
        fprintf(stderr, "FAILED malloc graph->root_nodes at graph_create\n");
        return NULL;
    }

    int32_t num_root_nodes = find_root_nodes(p_graph, b_incoming_edge_nodes);
    if(num_root_nodes == -1){
        fprintf(stdout, "FAILED find_root_nodes\n");
        return NULL;
    }

    p_graph->num_root_nodes = num_root_nodes;
    p_graph->num_intervals = num_intervals;

    bitmap_destroy(b_incoming_edge_nodes);
    fclose(fin);

#if DEBUG
    clock_t end = clock();
    fprintf(stdout, "GRAPH GENERATION took %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
#endif

    return p_graph;
}

static uint32_t parse_node_id(const char* buf) {
    uint32_t node_id = -1;
    sscanf(buf, "%u", &node_id);
    return node_id;
}

static void* thread_entry_point(void *thread_argument) {

    const uint16_t BUFF_SIZE = BUFFER_SIZE;
    const uint16_t NUM_LINES = MAX_THREADS_GRAPH;
    char lines[NUM_LINES][BUFF_SIZE];
    uint32_t node_ids[NUM_LINES];

    struct thread_arg *arg = (struct thread_arg*) thread_argument;
    FILE *fin = arg->fin;
    uint32_t num_intervals = arg->num_intervals;
    uint32_t tot_nodes = arg->tot_nodes;
    uint32_t *p_curr_iteration = arg->curr_iteration;
    Graph *p_graph = arg->graph;
    Bitmap *inc_edge_nodes = arg->inc_edge_nodes;
    pthread_spinlock_t *p_lock = arg->lock;

#if DEBUG
    fprintf(stdout, "> Thread_id %ld args: num_intervals %d tot_nodes %d iteration %d\n", pthread_self(), num_intervals, tot_nodes, *p_curr_iteration);
#endif
// avoid lock overhead when reading for the first time p_curr_iteration
        while(true){
            if(*p_curr_iteration < tot_nodes) {
                int max_valid_lines = NUM_LINES;
                for(int i = 0; i < NUM_LINES; i++) {
                    char* err = fgets(lines[i], BUFF_SIZE, fin);
                    if(err == NULL) {
                        max_valid_lines = i;
                        break;
                    }
                    node_ids[i] = parse_node_id(lines[i]);
                    if(node_ids[i] == -1) {
                        fprintf(stderr, "FAILED parsing node id at thread %ld\n", pthread_self());
                        fprintf(stderr, "Exiting...");
                        exit(3);
                    }
                }

                pthread_spin_lock(p_lock);
                uint32_t node_id = *p_curr_iteration;
                *p_curr_iteration = *p_curr_iteration + max_valid_lines;
                pthread_spin_unlock(p_lock);

                Node* base_node;
                if(max_valid_lines > 0) {
                    base_node = node_create_multiple(num_intervals, node_ids, max_valid_lines);
                    if(base_node == NULL) {
                        fprintf(stderr, "FAILED node_create_multiple at graph_create\n");
                        for(int j = 0; j < node_id; j++) {
                            free(p_graph->nodes[j]);
                        }
                        free(p_graph->nodes);
                        free(p_graph);
                        return NULL;
                    }
                    bitmap_set_bit(base_multi_nodes, node_ids[0]);
                }

                for(int i = 0; i < max_valid_lines; i++) {
                    Node* curr_node = base_node + i;
                    node_add_children(curr_node, lines[i]);
                    p_graph->nodes[node_ids[i]] = curr_node;
                    for(int j = 0; j < curr_node->num_children; j++) {
                        bitmap_set_bit(inc_edge_nodes, curr_node->children[j]);
                    }
                }

            }
            else {
                break;
            }
        }

        pthread_exit(NULL);
}

void graph_destroy(Graph *graph)
{
    if(graph == NULL)
        return;

    node_destroy_multiple(graph);
    // not great when we call multiple times graph_create before graph_destroy
    bitmap_destroy(base_multi_nodes);
    free(graph->root_nodes);
    free(graph->nodes);
    free(graph);
}

void graph_print_to_stdout(Graph* graph, bool verbose, uint32_t index_node)
{

    if(index_node == ALL_NODES){
        fprintf(stdout, "PRINTING GRAPH\n");
        int i = 0;
        while(i < graph->num_nodes){
            node_print_to_stdout(graph->nodes[i++], verbose);
        }

        fprintf(stdout, "%d ROOT NODES\n", graph->num_root_nodes);
        for(int i = 0; i < graph->num_root_nodes; i++){
            fprintf(stdout, "%d ", graph->root_nodes[i]);
        }

        fprintf(stdout, "\n");
        fprintf(stdout, "FINISHED PRINTING GRAPH\n\n");
    }
    else if (index_node > 0){
        node_print_to_stdout(graph->nodes[index_node], verbose);
    }

}

void node_print_to_stdout(Node* node, bool verbose){

    fprintf(stdout, "%d: ", node->id);
    for(int i = 0; i < node->num_children; i++) {
        printf("%d ", node->children[i]);
    }

    fprintf(stdout, "# ");

    if(verbose) {
        fprintf(stdout, "\nLABELS: ");
        for(int i = 0; i < node->num_intervals; i++) {
            fprintf(stdout, "[%d %d]", node->intervals[i].left, node->intervals[i].right);
        }

        fprintf(stdout, "\ntot children %d", node->num_children);
    }

    fprintf(stdout, "\n");
}

static int node_to_string(Node *node, char *buffer)
{
    sprintf(buffer, "%d: ", node->id);
    char mini[20];
    for(int i = 0; i < node->num_children; i++) {
        sprintf(mini, "%d ", node->children[i]);
        strcat(buffer, mini);
    }

    strcat(buffer, "#\n");
    return strlen(buffer);
}

bool graph_print_to_stream(char *graph_path_to, bool also_label, char *label_path_to, Graph *graph) {

    FILE *fout = fopen(graph_path_to, "wb");
    if(fout == NULL) {
        fprintf(stderr, "FAILED fopen at graph_print_to_file %s", graph_path_to);
        return false;
    }

    char buffer[10240 * 10];
    int curr_pos = 0;

    fprintf(fout, "%u\n", graph->num_nodes);
    int i = 0;
    while(i < graph->num_nodes) {
        for(int j = 0; j < 10 && i < graph->num_nodes; j++) {
            curr_pos += node_to_string(graph->nodes[i], &buffer[curr_pos]);
            i++;
        }
        fwrite(buffer, 1, curr_pos, fout);
        curr_pos = 0;
    }

    if(also_label)
        label_print_to_file(label_path_to, graph);

    return true;
};
