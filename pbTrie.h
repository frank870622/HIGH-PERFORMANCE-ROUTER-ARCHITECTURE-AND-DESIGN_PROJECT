#ifndef PBTRIE_H
#define PBTRIE_H

#define MAX_BITMAP 4096

typedef struct Lookup_Table Lookup_Table;

typedef struct PBTrie{
    char * bitmap;
    char node_name[100];
    int rule_id;
    int branch_couter;
    int bitmap_size;
    Lookup_Table ** lookup_table;
    struct PBTrie * father_pbTrie_node;
}PBTrie_node;

typedef struct Lookup_Table{
    PBTrie_node * pbTrie_node;
    Lookup_Table * down_lookup_table;
}Lookup_Table;

#endif