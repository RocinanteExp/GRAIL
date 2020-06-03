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
bitmap per roots
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
| Graph* initGraph(int d) | Graph.h | |
| Node* createNode(int d,int n_childrens) | Graph.h | |
| void destroyNode(Node* node)   | Graph.h | |
| void destroyGraph(Graph* node) | Graph.h | |
| void readGraph(char* graph_file,Graph* graph) | Graph.h	| |
| Label createLabel(int l,int r) | Label.h | D |
| int includeLabel(Label l1,Label l2)| Label.h | D |
| void initBitset(int n) | BitSet.h | |
| void destroyBitset(Bitset* bitset) | BitSet.h | |
| void setAll()| BitSet.h| |
| void set(size_t pos)| BitSet.h| |
| void reset(size_t pos)| BitSet.h| |
| void resetAll()| BitSet.h| |
| void flip(size_t pos)| BitSet.h| |
| void flipAll()| BitSet.h| |
| int isSet(size_t pos)| BitSet.h| | 
| int isAllSet()| BitSet.h| | 
| int isNoneSet()| BitSet.h| | 
| int isAnySet()| BitSet.h| | 
### 

### tips
per compilare con librerie personalizzate usare:
gcc main.c -o main Label.c -lm
vedere : https://gribblelab.org/CBootCamp/12_Compiling_linking_Makefile_header_files.html#org6e32cd2 per maggiori dettagli