#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "graph.h"
#include "bitmap.h"
#define DEBUG 1 
#define MAX_THREADS 5 

static void *start_thread(void *thread_argument);
static void *multi_line_read(void *thread_argument);
//Set the childrens of a node using the string red from the file
static void node_set_children(Node* node, char* str);

static void node_set_children2(Node *node, const char* str);

static Bitmap* non_root_nodes = NULL;
static struct thread_arg {
    FILE *fin;
    Graph *graph;
    uint32_t *cur_iteration;
    pthread_spinlock_t *lock;
    uint32_t num_intervals;
    uint32_t tot_nodes;
};

Node* node_create(uint32_t num_intervals, uint32_t node_id)
{
    Node* node = malloc(sizeof(Node));
    if(node == NULL)
        return NULL;

    node->intervals = malloc(num_intervals * sizeof(Label));
    for(int i = 0; i < num_intervals; i++)
        node->intervals[i]= label_init(UINT32_MAX,UINT32_MAX);

    node->id = node_id;
    node->children = NULL;
    node->num_children = 0;
    node->interval_bitmap = bitmap_create(num_intervals);
    node->num_intervals = num_intervals;
    
    return node;

}

Node* node_create_multiple(uint32_t num_intervals, uint32_t* node_ids, uint32_t num_nodes)
{
    Node* base_node = malloc(num_nodes * sizeof(Node));
    if(base_node == NULL) {
        fprintf(stdout, "malloc for base_node failed at node_create_multiple.\n%d a node_id", node_ids[0]);
        return NULL; 
    }

    Label* base_interval = malloc(num_nodes * num_intervals * sizeof(Label));
    if(base_interval == NULL) {
        fprintf(stdout, "malloc for interval failed at node_create_multiple.\n%d a node_id", node_ids[0]);
        return NULL; 
    }

    for(int i = 0; i < num_nodes ; i++) {
        Node* curr_node = base_node + i;
        curr_node->intervals = base_interval + i*num_intervals; 

        //curr_node->intervals = malloc(num_intervals * sizeof(Label));
        for(int j = 0; j < num_intervals; j++) {
            curr_node->intervals[j].left = UINT32_MAX;
            curr_node->intervals[j].right = UINT32_MAX;
        }

        curr_node->id = node_ids[i];
        curr_node->children = NULL;
        curr_node->num_children = 0;
        curr_node->interval_bitmap = bitmap_create(num_intervals);
        curr_node->num_intervals = num_intervals;
    }

    return base_node;

    
}

void node_destroy(Node* node)

{

    free(node->intervals);
    bitmap_destroy(node->interval_bitmap);
    free(node->children);
    free(node);

}
static void node_set_children2(Node* node, const char* str)
{
    uint32_t str_length = strlen(str);
    int begin_index = 0; 
    uint32_t numbers[1024] = {0};
    uint32_t next = 0;
    for(int i = 0; i < str_length; i++) {
        if(str[i] >= 48 && str[i] <= 57) { // 48 == '0'; 57 == '9'
            if(begin_index == -1) {
                begin_index = i;
            }
        }
        else if (str[i] == 32 || str[i] == 58) { // 32 == space; 58 == :
            if(begin_index != -1) {
                numbers[next] = atoi(str + begin_index);
                next++;
                if(next >= 1024) {
                    printf("OVERFLOWWWWWWWWWWWWWWWWWWWWWWWWW\n");
                    exit(1);
                }
                begin_index = -1;
            }
        }
        else if (str[i] == 35 || str[i] == 13 || str[i] == 10) { // 35 = #; 13 = carriage return; 10 = new line feed
            break;
        }
        else {
            printf("UNKNOWN CHARACTER %d\n", str[i]);
        }
    }

    next = next - 1; // skip the node id
    node->children = malloc(next * sizeof(uint32_t));
    if(node->children == NULL) {
        fprintf(stderr, "failed malloc on node_set_children2\n");
        exit(1);
    }
    memcpy(node->children, &numbers[1], next * sizeof(uint32_t)); 
    node->num_children = next;
    //for(int i = 0; i < next; i++) {
    //   printf("children [%d] = %u number[%d] = %u\n", i, node->children[i], i, numbers[i]);
    //}

    //free(numbers - 1);
}
static void node_set_children(Node* node, char* str)
{
    uint32_t n=0;
    uint32_t i=0;
    uint32_t str_length = strlen(str);
    char* s=malloc(sizeof(char)*(str_length+1));
    char* tok;
    char * rest;
    //char* context;
    if(strcpy(s, str) == NULL)
    {
        fprintf(stderr, "ERROR: strcpy set_children\n");
        return; 
    }
    rest=s;
    // finding how many children does a node have
    tok = strtok_r(s, ": #\n\r",&rest);
    while((tok = strtok_r(NULL, ": #\n\r",&rest)) != NULL)
    {
        n++;
    } 

    node->children = malloc(n*sizeof(uint32_t));
    if(node->children == NULL) {
        fprintf(stderr, "failed malloc on node_set_children\n");
        exit(1);
    }

    if(strcpy(s, str) == NULL)
    {
        fprintf(stderr, "ERROR: strcpy set_children\n");
        return; 
    }
    rest=s;
    tok = strtok_r(s, ": #\n\r",&rest);
    while((tok = strtok_r(NULL, ": #\n\r",&rest)) != NULL && i < n)
    {
        node->children[i] = (unsigned int) atoi(tok);
        i++;
    }

    node->num_children = n;

}


Graph* graph_create(char *filepath, int num_intervals) {

    static uint32_t cur_iteration = 0;
    static pthread_spinlock_t lock;
    const uint16_t BUFF_SIZE = 8192; 

    FILE *fin;
    fin = fopen( filepath, "r");
    if(fin == NULL){
        fprintf(stdout, "ERROR opening file %s at graph_create\n", filepath);
        return NULL;
    }

    Graph *p_graph = malloc(sizeof(Graph));
    if(p_graph == NULL){
        fprintf(stdout, "ERROR on malloc of 'graph' at graph_create\n");
        return NULL;
    }

    // parsing first line 
    char curr_line[BUFF_SIZE];
    fgets(curr_line, BUFF_SIZE, fin);
    uint32_t num_nodes;
    sscanf(curr_line, "%d", &num_nodes);

    p_graph->num_nodes = num_nodes;
    p_graph->num_intervals = num_intervals;

    p_graph->nodes = malloc(num_nodes * sizeof(Node*));
    if(p_graph->nodes == NULL){
        free(p_graph);
        fprintf(stdout, "ERROR on malloc of 'graph->nodes' at graph_create\n");
        return NULL;
    }

    non_root_nodes = bitmap_create(num_nodes);

#if DEBUG
    clock_t start = clock();
#endif 

    int err = pthread_spin_init(&lock, 0);
    if(err != 0){
        fprintf(stderr, "ERROR: pthread_spin_init at graph_create\n");
        exit(1);
    };

    cur_iteration = 0;
    pthread_t thread_ids[MAX_THREADS];
    struct thread_arg thread_arg = {
        .fin = fin, .graph = p_graph, .num_intervals = num_intervals,
        .cur_iteration = &cur_iteration, .tot_nodes = num_nodes, .lock = &lock
    }; 

    for(int i = 0; i < MAX_THREADS; i++) {
        int err = pthread_create(&thread_ids[i], NULL, multi_line_read, (void*) &thread_arg); 
        if(err != 0) {
            fprintf(stderr, "ERROR: pthread_create %d", i);
            exit(2);
        };
    }

    for(int i = 0; i < MAX_THREADS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    pthread_spin_destroy(&lock);

#if DEBUG
    clock_t end = clock();
    fprintf(stdout, "GRAPH GENERATION took %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
#endif

    //finding the root nodes of the graph
    uint32_t next = 0;
    uint32_t MAGIC_NUMBER = 1024;
    p_graph->root_nodes = malloc(MAGIC_NUMBER * sizeof(uint32_t));
    for(int i = 0; i < p_graph->num_nodes; i++) {
        if(!bitmap_test_bit(non_root_nodes, i)) {
            p_graph->root_nodes[next++] = i;
        }
        if(next >= MAGIC_NUMBER) {
            MAGIC_NUMBER *= 2;
            //fprintf(stdout, "REALLOCCCCCCCCCCCCCCCCC\n");
            p_graph->root_nodes = realloc(p_graph->root_nodes, MAGIC_NUMBER * sizeof(uint32_t));
        }
    }
    p_graph->num_root_nodes = next;

    bitmap_destroy(non_root_nodes);
    fclose(fin);

    return p_graph;

}

static void *start_thread(void *thread_argument) {

    uint16_t BUFF_SIZE = 8192; 
    char lines[BUFF_SIZE];

    struct thread_arg *arg = (struct thread_arg*) thread_argument;
    FILE *fin = arg->fin;
    uint32_t num_intervals = arg->num_intervals;
    uint32_t tot_nodes = arg->tot_nodes;
    uint32_t *p_cur_iteration = arg->cur_iteration;
    Graph *p_graph = arg->graph;
    pthread_spinlock_t *p_lock = arg->lock;

#if DEBUG
    fprintf(stdout, "args num_intervals %d tot_nodes %d iteration %d\n", num_intervals, tot_nodes, *p_cur_iteration);
#endif
    while(1){
        pthread_spin_lock(p_lock);

        if(*p_cur_iteration < tot_nodes){
            fgets(lines, BUFF_SIZE, fin);
            uint32_t node_id = (*p_cur_iteration)++;
            pthread_spin_unlock(p_lock);

            Node* curr_node = node_create(num_intervals, node_id);
            if(curr_node == NULL){
                fprintf(stderr, "ERROR: failed node_create at iteration %d of graph_create\n", node_id);
                for(int j = 0; j < node_id; j++){
                    free(p_graph->nodes[j]);
                }
                free(p_graph->nodes);
                free(p_graph);
                return NULL;
            }   
            //pthread_spin_lock(p_lock);
            node_set_children(curr_node, lines);
           //pthread_spin_unlock(p_lock);
            p_graph->nodes[node_id] = curr_node;

            // set the nodes that have incomings edges
            for(int i = 0; i < curr_node->num_children; i++)
                bitmap_set_bit(non_root_nodes, curr_node->children[i]);

        }else{
            pthread_spin_unlock(p_lock);
            break;
        }

    }

    return (void*) 1;

}

static uint32_t parse_node_id(char* buf) {
    uint32_t node_id = -1;
    sscanf(buf, "%d", &node_id);
    //printf("line: %sNODE_ID %u\n", buf, node_id);
    return node_id;
}

static void *multi_line_read(void *thread_argument) {

    const uint16_t BUFF_SIZE = 8192; 
    const uint8_t NUM_LINES = MAX_THREADS;
    char lines[NUM_LINES][BUFF_SIZE];
    uint32_t node_ids[NUM_LINES];

    struct thread_arg *arg = (struct thread_arg*) thread_argument;
    FILE *fin = arg->fin;
    uint32_t num_intervals = arg->num_intervals;
    uint32_t tot_nodes = arg->tot_nodes;
    uint32_t *p_cur_iteration = arg->cur_iteration;
    Graph *p_graph = arg->graph;
    pthread_spinlock_t *p_lock = arg->lock;

#if DEBUG
    fprintf(stdout, "thread_id %ld args: num_intervals %d tot_nodes %d iteration %d\n", pthread_self(), num_intervals, tot_nodes, *p_cur_iteration);
#endif
        while(true){
            if(*p_cur_iteration < tot_nodes) {

                int max_valid_nodes = NUM_LINES;
                for(int i = 0; i < NUM_LINES; i++) {
                    char* err = fgets(lines[i], BUFF_SIZE, fin);
                    if(err == NULL) {
                        max_valid_nodes = i;
                        break;
                    }
                    node_ids[i] = parse_node_id(lines[i]);
                }

                pthread_spin_lock(p_lock);
                uint32_t node_id = *p_cur_iteration;
                *p_cur_iteration = *p_cur_iteration + max_valid_nodes;
                pthread_spin_unlock(p_lock);

                Node* base_node;
                if(max_valid_nodes > 0) { 
                    base_node = node_create_multiple(num_intervals, node_ids, max_valid_nodes);
                    if(base_node == NULL){
                        fprintf(stderr, "ERROR: failed node_create_multiple of graph_create\n");
                        for(int j = 0; j < node_id; j++){
                            free(p_graph->nodes[j]);
                        }
                        free(p_graph->nodes);
                        free(p_graph);
                        return NULL;
                    }   
                }

               for(int i = 0; i < max_valid_nodes; i++) { 
                   Node* curr_node = base_node + i;
                   node_set_children2(curr_node, lines[i]);
                   p_graph->nodes[node_ids[i]] = curr_node; 
                   for(int j = 0; j < curr_node->num_children; j++) {
                       bitmap_set_bit(non_root_nodes, curr_node->children[j]);
                   }
               }

            }
            else {
                break;
            }

        }

        pthread_exit(NULL);

}

void graph_destroy(Graph *graph){

    if(graph == NULL)
        return;

    for(int i = 0; i < graph->num_nodes; i++){
        node_destroy(graph->nodes[0]);
    }

    free(graph->root_nodes);
    free(graph->nodes);
    free(graph);

}

void graph_print(Graph *graph, bool verbose, uint32_t index_node){


    if(index_node == -1){
        fprintf(stdout, "PRINTING GRAPH\n");
        int i = 0;
        while(i < graph->num_nodes){
            node_print(graph->nodes[i++], verbose);
        }

        fprintf(stdout, "%d ROOT NODES\n", graph->num_root_nodes);
        for(int i = 0; i < graph->num_root_nodes; i++){
            fprintf(stdout, "%d ", graph->root_nodes[i]);
        }

        fprintf(stdout, "\n");
        fprintf(stdout, "FINISHED PRINTING GRAPH\n");
    }
    else if (index_node > 0){
        node_print(graph->nodes[index_node], verbose); 
    }


}

//TODO implement a "verbose" version of printing a node
void node_print(Node *node, bool verbose){

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

void labels_print(Graph *graph)
{
    int i=0;
    printf("PRINT GRAPH LABELS\n");
    for(i=0;i<graph->num_nodes;i++)
    {
        Node* node = graph->nodes[i];
        int j=0;
        printf("#%d :",node->id);
        for(j=0;j<graph->num_intervals;j++)
        {
            printf(" (%d,%d)",node->intervals[j].left,node->intervals[j].right);
        }
        printf(" #\n");
    }
}

static int node_print_out(Node *node, FILE *fout){

    fprintf(fout, "%d: ", node->id);
    for(int i = 0; i < node->num_children; i++) {
        int err = fprintf(fout, "%d ", node->children[i]);
        if(err < 0) {
            return err;
        }
    }
    
    int err = fprintf(fout, "#\n");
    if(err < 0) {
        return err;
    }

    return 1;
}

bool graph_print_to_file(char *filename, Graph *graph) {
    FILE *fout = fopen(filename, "w");
    if(fout == NULL) {
        fprintf(stderr, "fopen failed at graph_print_to_file %s", filename);
        return false;
    }

    fprintf(fout, "%u\n", graph->num_nodes);
    int i = 0;
    while(i < graph->num_nodes) {
        int err = node_print_out(graph->nodes[i], fout);
        if(err < 0) {
            fprintf(stderr, "failed node_print_out at graph_print_to_file: i %d\n", i);  
            return false;
        }
        i++;
    }

    return true;
};


bool label_print_to_file(char *filename, Graph *graph) {
    FILE *fout = fopen(filename, "w");
    if(fout == NULL) {
        fprintf(stderr, "fopen failed at label_print_to_file %s", filename);
        false;
    }

    const uint32_t tot_nodes = graph->num_nodes;
    const uint32_t tot_intervals = graph->num_intervals;
    for(uint32_t i = 0; i < tot_nodes; i++) {
        Node* node = graph->nodes[i];

        int err = fprintf(fout, "%u: ", node->id);
        if(err < 0) {
            fprintf(stderr, "failed label_print_out at graph_print_to_file: #%u\n", i);  
            return false;
        }

        for(uint32_t j = 0; j < tot_intervals; j++) {
            err = fprintf(fout, "[%u, %u] ", node->intervals[j].left, node->intervals[j].right);
            if(err < 0) {
                fprintf(stderr, "failed label_print_out at graph_print_to_file: [...] with j %u\n", j);  
                return false;
            }
        }

        err = fprintf(fout, "\n");
        if(err < 0) {
            fprintf(stderr, "failed label_print_out at graph_print_to_file: \\n\n");  
            return false;
        }
    }

    return true;

};