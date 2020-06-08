# SDPProjectQ2 Organizzazione struct e files

## Structs
typedef struct bitset_s{
int* bitset;
size_t size;
} BitSet
typedef struct label_s{
int left,right;
} Label;
typedef struct node_s{
uint_8 id;
struct node_s ** childrens;
Label* intervals;
bitmap d bits
} Node;

typedef struct{
int N; dimensione
Node* nodes;
int* roots;
int n_roots;
} Graph;


## Tabella .h
### Struct
### 
| Struct | .h |
| -------- | ---- |
| Graph | Graph.h |
| Node  | Graph.h |
| Label | Label.h |
| BitSet | BitSet.h |
### 
### Funzioni
Stato: F= Finito, P =Parziale, in lavorazione
| Funzione | .h | Stato|
| ---------- | ---- | --------- |
| Graph* initGraph(int n_nodes,int d) | Graph.h | |
| Node* createNode(int d,int n_childrens) | Graph.h | F |
| Graph* graph_create(char* graph_file)|Grapf.h| |
| void destroyNode(Node* node)   | Graph.h | F |
| void destroyGraph(Graph* node) | Graph.h | |
| Label createLabel(int l,int r) | Label.h | F |
| int includeLabel(Label l1,Label l2)| Label.h | F |
| void initBitset(int n) | BitSet.h | F |
| void destroyBitset(Bitset* bitset) | BitSet.h | F |
| void setAll()| BitSet.h| F |
| void set(size_t pos)| BitSet.h| F |
| void reset(size_t pos)| BitSet.h| F |
| void resetAll()| BitSet.h| F |
| void flip(size_t pos)| BitSet.h| |
| void flipAll()| BitSet.h| |
| int isSet(size_t pos)| BitSet.h| F | 
| int isAllSet()| BitSet.h| | 
| int isNoneSet()| BitSet.h| | 
| int isAnySet()| BitSet.h| | 
### 

### tips
per compilare con librerie personalizzate usare:
gcc main.c -o main Label.c -lm
vedere : https://gribblelab.org/CBootCamp/12_Compiling_linking_Makefile_header_files.html#org6e32cd2 per maggiori dettagli
per compilare i tests esempio
gcc -Wall label_test.c -o test Label.c -lcheck -lm -lpthread -lrt -lsubunit