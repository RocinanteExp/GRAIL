#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static void generate_query (int, int, char *);
static void seed_ran (void);

int main(int argc, char **argv) {

    if(argc != 4) {
        fprintf(stderr, "Usage: %s <max_node_id> <num_queries> <filename>\n", argv[0]);
        exit(-1);
    }
    
    seed_ran();
    int tot_nodes = atoi(argv[1]);
    int query_n = atoi(argv[2]);
    char *name = argv[3];
    generate_query(tot_nodes, query_n, name);

}

static void seed_ran (void) {
   srand( ( unsigned short ) time( NULL ) );
}

static int ran (int n1, int n2) {
   int k, r;
   k = n2 - n1;
   r = n1 + rand() % k;
   return r;
}

static void generate_query (int v_n, int query_n, char *name)
{
   int i, v1, v2;
   FILE *fp;

   fp = fopen (name, "w");
   if (fp == NULL) {
      printf( "Unable to open file %s for writing.\n", name );
      return;
   }
   fprintf (stdout, "Writing (%d) queries to file %s\n", query_n, name);

   for (i=0; i<query_n; i++) {
     v1 = ran (0, v_n-1);
     v2 = ran (0, v_n-1);
     if (v2>v1) {
       fprintf (fp, "%d %d\n", v1, v2);
     } else {
       fprintf (fp, "%d %d\n", v2, v1);
     }
   }

   fclose (fp);

   return;
}
