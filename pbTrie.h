#ifndef PBTRIE_H
#define PBTRIE_H

#define MAX_BITMAP 4096
#define HASH_TABLE_SIZE 1000
#define MAX_CACHE_NUM 10

typedef struct Lookup_Table Lookup_Table;
typedef struct PBTrie PBTrie_node;
typedef struct FingerPrint FingerPrint;
typedef struct PBC_cache PBC_cache;
typedef struct Hash_Table Hash_Table;

typedef struct PBTrie{
    char * bitmap;
    char node_name[100];
    int rule_id;
    int branch_couter;
    int bitmap_size;
    Lookup_Table ** lookup_table;
    PBTrie_node * father_pbTrie_node;
}PBTrie_node;

typedef struct Lookup_Table{
    PBTrie_node * pbTrie_node;
    Lookup_Table * down_lookup_table;
}Lookup_Table;

typedef struct FingerPrint{
    char fingerprint_string[100];
    FingerPrint * next_fingerprint;
}FingerPrint;

typedef struct PBC_cache{
    FingerPrint * cache_fingerprint;
    int rule_id;
    int fingerprint_len;
    PBTrie_node * pbTrie_pointer;
    PBC_cache * pre_PBC_cahce;
    PBC_cache * down_PBC_cache;
}PBC_cache;

typedef struct Hash_Table{
    PBC_cache * cache_pointer;
    Hash_Table * down_hash_table;
}Hash_Table;

#endif