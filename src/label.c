#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include "label.h"
#include "graph.h"
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

struct thread_argument {
    Graph* graph;
    uint32_t idx;
    uint32_t rank;
};

static void* thread_generate_interval(void* thread_argument);

#if !TEST
static 
#endif
void vec_random_shuffle(uint32_t *vec, uint32_t size)
{
    uint32_t n = size, j = 0, i = 0, step = 0;

    if(size > 1000000)
        n = 1000000;

    unsigned int si = rand();  
    unsigned int sj = rand();

    for(step = 0; step < n; ++step)
    {
        i = rand_r(&si) % size;
        j = rand_r(&sj) % size;
        uint32_t tmp = vec[i];
        vec[i] = vec[j];
        vec[j] = tmp;
    }
}

#if !TEST
static 
#endif
void randomized_visit(Graph* graph, Bitmap* visited_nodes, uint32_t node_id, uint32_t idx, uint32_t* rank)
{
    int j = 0;
    unsigned int state = rand(); 
    uint32_t num_childrens = 0;
    uint32_t rc = UINT32_MAX;

    if(bitmap_test_bit(visited_nodes,node_id))
    {
        return;
    }

    Node *node = graph->nodes[node_id];
    bitmap_set_bit(visited_nodes, node_id);
    if(node->num_children>0)

#if TEST
        j = 0;
#else
        j = rand_r(&state)%node->num_children;
#endif

    while(num_childrens < node->num_children)
    {
        num_childrens++;
        randomized_visit(graph, visited_nodes, node->children[j], idx, rank);
        j = (j+1)%node->num_children;
    }
    
    bool first = true; 
    for(j=0; j<node->num_children; j++)
    {
        if(first){
            first = false;
            rc = MIN(rc,graph->nodes[node->children[j]]->intervals[idx].left);
            continue;
        }

        int tmp_rc = MIN(rc,graph->nodes[node->children[j]]->intervals[idx].left);
        if(tmp_rc < rc)
            rc = tmp_rc;
    }

    node->intervals[idx].left = MIN(*rank,rc);
    node->intervals[idx].right = *rank;
    *rank = *rank+1;
}

// thread fuction fo setting intervals of the labels of the given index
static void *thread_generate_interval(void *thread_argument)
{
    struct thread_argument *arg = (struct thread_argument *) thread_argument;
    Graph *graph = arg->graph;
    uint32_t idx = arg->idx;
    uint32_t rank = arg->rank;
    int i = 0;

    uint32_t *roots = malloc(graph->num_root_nodes*sizeof(uint32_t));
    if(roots == NULL)
    {
        fprintf(stderr, "FAILED malloc roots at thread_generate_interval\n");
        exit(-1);
    } 
    for(i = 0; i < graph->num_root_nodes; i++)
        roots[i] = graph->root_nodes[i];
#if !TEST
    vec_random_shuffle(roots, graph->num_root_nodes);
#endif

    Bitmap* visited_nodes = bitmap_create(graph->num_nodes);
    if(visited_nodes == NULL)
    {
        fprintf(stderr, "FAILED bitmap_create of visited_notes at thread_generate_interval\n");
        exit(-1);
    }

    for(i = 0; i < graph->num_root_nodes; i++)
        randomized_visit(graph, visited_nodes, roots[i], idx, &rank);

    bitmap_destroy(visited_nodes);
    free(roots);

    pthread_exit((void*) 0);
}

void label_generate_random_labels(Graph* graph)
{
#if DEBUG
    fprintf(stdout, "GENERATING LABELS...\n");
    clock_t start = clock();
#endif

    srand(time(NULL));
    uint32_t idx = 0;
    pthread_t *tids = malloc(graph->num_intervals * sizeof(pthread_t));
    struct thread_argument *args = malloc(graph->num_intervals * sizeof(struct thread_argument));
    if(args == NULL) {
        fprintf(stderr, "FAILED malloc of thread args at label_generate_random_labels\n");
        exit(-2);
    }
    
    // one thread generate a single interval
    for(idx = 0; idx < graph->num_intervals; idx++)
    {
        args[idx].graph = graph;
        args[idx].idx = idx;
        args[idx].rank = 1; 
        int err = pthread_create(&tids[idx], NULL, thread_generate_interval, (void*)&args[idx]);
        if(err != 0)
        {
            fprintf(stderr, "FAILED pthread_create %d at label_generate_random_labels", idx);
            exit(-2);
        }
    }

    for(idx = 0; idx < graph->num_intervals; idx++)
    {
        int err = pthread_join(tids[idx], NULL);
        if(err != 0)
        {
            fprintf(stderr, "FAILED pthread_join %d at label_generate_random_labels", idx);
            exit(-2);
        }
    }

    free(args);
    free(tids);

#if DEBUG
    clock_t end = clock();
    fprintf(stdout, "LABEL GENERATION took %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
#endif

}

bool label_print_to_file(char *filename, Graph *graph)
{
    FILE *fout = fopen(filename, "w");
    if(fout == NULL) {
        fprintf(stderr, "fopen failed at label_print_to_file %s", filename);
        return false;
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
                fprintf(stderr, "FAILED label_print_out at graph_print_to_file: [...] with j %u\n", j);
                return false;
            }
        }

        err = fprintf(fout, "\n");
        if(err < 0) {
            fprintf(stderr, "FAILED label_print_out at graph_print_to_file: \\n\n");
            return false;
        }
    }

    fclose(fout);
    return true;
};

Label label_init(uint32_t l,uint32_t r)
{
    Label x;
    x.left = l;
    x.right = r;
    return x;
}

bool label_include(Label l1,Label l2)
{
    if(l1.left < l2.left || l1.right > l2.right)
        return false;
    return true;
}
