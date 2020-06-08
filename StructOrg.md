# SDPProjectQ2 Organizzazione struct e files
```
## Structs
typedef struct bitmap_s{
    uint32_t* bitset;
    size_T length;
    size_t size;
} Bitmap

typedef struct label_s{
    int left,right;
} Label;

typedef struct node_s{
  uint32_t id;
  uint32_t* children;
  uint32_t num_children;
  Label* intervals;
  Bitmap* interval_bitmap;
} Node;

typedef struct{
    uint32_t num_nodes;
    uint32_t num_root_nodes;
    uint32_t num_intervals;
    uint32_t* root_nodes;
    Node** nodes;
} Graph;
```

## Tabella .h
### Struct
### 
| Struct | .h |
| -------- | ---- |
| Graph | graph.h |
| Node  | graph.h |
| Label | label.h |
| Bitmap | bitmap.h |
### 
### Funzioni
Stato: F = Finito, P = Parziale, in lavorazione
D = Deprecato
| Funzione                                          | .h        | Stato |
| ---------- | ---- | --------- |
| Graph* graph_init(char* graph_file)               | Grapf.h   | F |
| Graph* graph_create(int n_nodes,int d)            | Graph.h   | F |
| void graph_destroy(Graph* node)                   | Graph.h   | F |
| Node* node_create(int d,int n_childrens)          | Graph.h   | F |
| void node_destroy(Node* node)                     | Graph.h   | F |
| Label label_init(int l,int r)                     | Label.h   | F |
| bool label_include(Label l1,Label l2)             | Label.h   | F |
| Bitmap* bitmap_create(size_t num_bits)            | Bitset.h  | F | 
| void bitmap_destroy(Bitmap* bitmap)               | BitSet.h  | F |
| void bitmap_set_all(Bitmap* bitmap)               | BitSet.h  | F |
| void bitmap_set_bit(Bitmap* bitmap, size_t pos)   | BitSet.h  | F |
| void bitmap_clear_bit(Bitmap* bitmap, size_t pos) | BitSet.h  | F |
| void bitmap_clear_all(Bitmap* bitmap))            | BitSet.h  | F |
| int bitmap_test_bit(Bitmap* bitmap, size_t pos)   | BitSet.h  | F | 
| int isAllSet()                                    | BitSet.h  | | 
| int isNoneSet()                                   | BitSet.h  | | 
| int isAnySet()                                    | BitSet.h  | | 
| void flip(size_t pos)                             | BitSet.h  | |
| void flipAll()                                    | BitSet.h  | |
### 

### tips
per compilare con librerie personalizzate usare:
gcc main.c -o main Label.c -lm
vedere : https://gribblelab.org/CBootCamp/12_Compiling_linking_Makefile_header_files.html#org6e32cd2 per maggiori dettagli
per compilare i tests esempio
gcc -Wall label_test.c -o test Label.c -lcheck -lm -lpthread -lrt -lsubunit