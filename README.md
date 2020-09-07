# Assignment Quer Project 2

Group # 27  
Students: Giovanni Tangredi s276086 , Francesco Xia s276509

## Suggested action for compiling and executing the project

execute the following commands:
```bat
make
cd bin
./main graph_file n queue_file
```
for the tests:
```bat
make test_node
cd bin
./test 
```
see makefile for the other tests

## Folder Stucture
``` bash
SDPProjectQ2
├── README.md
├── bin
│   ├── main
│   └── test
├── include
│   ├── bitmap.h
│   ├── constants.h
│   ├── graph.h
│   ├── label.h
│   └── query.h
├── makefile
├── src
│   ├── bitmap.c
│   ├── graph.c
│   ├── label.c
│   ├── main.c
│   └── query.c
└── test
    ├── bitmap_test.c
    ├── graph_test.c
    ├── input 
    │   ├── grafo20.gra
    │   ├── grafo20.png
    │   ├── grafo20_25.que
    │   ├── grafo_con_rango.png
    │   └── large
    │       ├── arXiv_sub_6000-1.gra
    │       ├── cit-Patents.scc.gra
    ├── label_test.c
    ├── node_test.c
    ├── output
    │   └── query_output.txt
    └── query_test.c
```
the src folder contains all the source files for the project.
the include folder contains all the header files needed for the project, in particular the constant.h file contains all the important constants needed for the project.
the executable files will be created in the bin folder after the make
the test folder contains all the source files for the tests test done during the project and some test input used during the developtment. test/input was used during the test for the input files , similar for output.
The tests were created using the Check.h Framework.


## Program output

the program generate a query_output.txt file that is structured the following way for each line:
source_id destination_id 0(not reachable)/1(reachable)
this structure is compatible with the test structure for the GRAIL program so that the result of the program can be tested using  GRAIL.
Also the output file with the labels can be printed with the following format:
node_id: label1 label2 ...
the label are rappresented this way : [left,right]

## Data Stuctures 
The graph is represented using a data structure similar to an Adjacency list.  A vector of all nodes with the node_id used as the index is used for storing all the nodes then each node has a dynamically allocated vector with the ids of all the children.
All the ids of the roots nodes are saved in another vector.

## Adopted strategies
For the labeling the generation of each label is executed in parallel because each label generation can be executed independently from the others, the only exception  is the  random shuffle of the root vector, for this purpose each thread create a copy of the root vector. the child vector of each node is the traverse sequentially starting from a random generated index. In this way int he worst case scenario we complexity is reduced from O(label_generation_time*D) to O(max(label-generation_time)).

For the query a similar strategy is used, the file is divided in 4 equal block and the a thread is launched for each block.