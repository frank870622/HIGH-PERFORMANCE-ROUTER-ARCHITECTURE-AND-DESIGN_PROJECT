#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"pbTrie.h"


//////////////////////
static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

unsigned long long int cycle_begin = 0, cycle_end = 0, total_build_cycle = 0, total_search_cycle = 0, total_delete_cycle = 0, total_insert_cycle = 0;
unsigned long long int total_build_count = 0, total_search_count = 0, total_delete_count = 0, total_insert_count = 0;
unsigned long long int max_build_cycle = 0, max_search_cycle = 0, max_delete_cycle = 0, max_insert_cycle = 0;
unsigned long long int min_build_cycle = 10000000, min_search_cycle = 10000000, min_delete_cycle = 10000000, min_insert_cycle = 10000000;
//////////////////////
unsigned int BKDRHash(const char* str, unsigned int len)
{
   unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (hash * seed) + (*str);
   }

   return hash;
}
/* End Of BKDR Hash Function */

//////////////////////
// bitmap
int get_hash_value(const char* str, unsigned int string_len, int bitmap_size){
    unsigned int hash_value = BKDRHash(str, strlen(str));
    int index_of_bitmap = hash_value % bitmap_size;

    return index_of_bitmap;
}

int accumulate_i_of_bitmap(char * bitmap, int accumulate_bitmap_end){
    if (bitmap == NULL) return 0;

    int count = 0;

    for (int i = 0; i < accumulate_bitmap_end; ++i){
        if (bitmap[i] == 1){
            ++count;
        }
    }
    return count;
}

int get_pow_value_of_2(int branch_number){
    int bitmap_size = 1;
    while(bitmap_size <=  2 * branch_number){
        bitmap_size = 2 * bitmap_size;
        if (bitmap_size >= MAX_BITMAP){
            return MAX_BITMAP;
        }
    }
    return bitmap_size;
}

//////////////////////
PBTrie_node * pbTrie_root;
PBC_cache * PBC_cache_head;
Hash_Table ** hash_table;
int now_cache_number = 0;

//status
int number_of_pbTrie_node = 0;
int max_branch_counter = 0;
int branch_counter_counter = 0;
int bitmap_size_counter = 0;
int number_of_level_pbTire_node[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
////////


PBTrie_node * create_pbTrie_node(){
    PBTrie_node * new_node = (PBTrie_node *)malloc(sizeof(PBTrie_node));
    new_node -> bitmap = NULL;
    new_node -> node_name[0] = 0;
    new_node -> rule_id = 0;
    new_node -> branch_couter = 0;
    new_node -> bitmap_size = 0;
    new_node -> lookup_table = NULL;
    new_node -> father_pbTrie_node = NULL;

    return new_node;
}

Lookup_Table * create_lookup_table(){
    Lookup_Table * new_lookup_table = (Lookup_Table *)malloc(sizeof(Lookup_Table));
    new_lookup_table -> pbTrie_node = NULL;
    new_lookup_table -> down_lookup_table = NULL;
    return new_lookup_table;
}

Lookup_Table ** create_lookup_table_pointer_array(int array_len){
    //printf("in \"create_lookup_table_pointer_array\", MSG : array_len is : %d\n", array_len);
    Lookup_Table ** new_lookup_table = (Lookup_Table **)malloc(array_len * sizeof(Lookup_Table *));
    for(int i = 0; i < array_len; ++i){
        new_lookup_table[i] = NULL;
    }

    return new_lookup_table;
}

FingerPrint * create_fingerprint(){
    FingerPrint * new_fingerprint = (FingerPrint *)malloc(sizeof(struct FingerPrint));
    new_fingerprint -> fingerprint_string[0] = 0;
    new_fingerprint -> next_fingerprint = NULL;
    return new_fingerprint;
}

PBC_cache * create_PBC_cache(){
    PBC_cache * new_PBC_cache = (PBC_cache *)malloc(sizeof(struct PBC_cache));
    new_PBC_cache -> cache_fingerprint = NULL;
    new_PBC_cache -> rule_id = 0;
    new_PBC_cache -> fingerprint_len = 0;
    new_PBC_cache -> pbTrie_pointer = NULL;
    new_PBC_cache -> pre_PBC_cahce = NULL;
    new_PBC_cache -> down_PBC_cache = NULL;
    return new_PBC_cache;
}

Hash_Table * create_hash_table(){
    Hash_Table * new_hash_table = (Hash_Table *)malloc(sizeof(struct Hash_Table));
    new_hash_table -> cache_pointer = NULL;
    new_hash_table -> down_hash_table = NULL;
    return new_hash_table;
}

void init_PBC_cache_header(){
    PBC_cache_head = (PBC_cache *)malloc(sizeof(struct PBC_cache));
    PBC_cache_head -> cache_fingerprint = NULL;
    PBC_cache_head -> rule_id = 0;
    PBC_cache_head -> fingerprint_len = 0;
    PBC_cache_head -> pbTrie_pointer = NULL;
    PBC_cache_head -> pre_PBC_cahce = NULL;
    PBC_cache_head -> down_PBC_cache = NULL;
}

void init_hash_table(){
    hash_table = (Hash_Table **)malloc(HASH_TABLE_SIZE * sizeof(struct Hash_Table * ));
    for(int i = 0; i < HASH_TABLE_SIZE; ++i){
        hash_table[i] = NULL;
    }
}


void print_pbTrie_node_information(PBTrie_node * input_node){
    if (input_node == NULL) printf("in \"print_pbTrie_node_information\", MSG : pbTrie node is NULL\n");
    else                    printf("in \"print_pbTrie_node_information\", MSG : pbTrie node is not NULL\n");

    if (input_node -> bitmap == NULL) printf("in \"print_pbTrie_node_information\", MSG : pbTrie bitmap is NULL\n");
    else                              printf("in \"print_pbTrie_node_information\", MSG : pbTrie bitmap is not NULL\n");

    if (input_node -> node_name == NULL) printf("in \"print_pbTrie_node_information\", MSG : pbTrie node_name is NULL\n");
    else                                 printf("in \"print_pbTrie_node_information\", MSG : pbTrie node_name : %s\n", input_node -> node_name);

    if (input_node -> lookup_table == NULL) printf("in \"print_pbTrie_node_information\", MSG : pbTrie lookup_table is NULL\n");
    else                                    printf("in \"print_pbTrie_node_information\", MSG : pbTrie lookup_table is not NULL\n");

    printf("in \"print_pbTrie_node_information\", MSG : pbTrie rule_id : %d\n", input_node -> rule_id);
    printf("in \"print_pbTrie_node_information\", MSG : pbTrie branch_couter : %d\n", input_node -> branch_couter);
    printf("in \"print_pbTrie_node_information\", MSG : pbTrie bitmap_size : %d\n", input_node -> bitmap_size);
}

void get_pbTrie_status(PBTrie_node * now_node, int level_number){
    if (now_node == NULL){
        return;
    }
    else{
        //add node count
        ++number_of_pbTrie_node;
        if (now_node -> branch_couter > max_branch_counter)
            max_branch_counter = now_node -> branch_couter;
        else;
        branch_counter_counter += now_node -> branch_couter;
        bitmap_size_counter += now_node -> bitmap_size;
        number_of_level_pbTire_node[level_number] += 1;


        //recuresively to child
        int lookup_table_len = accumulate_i_of_bitmap(now_node->bitmap, now_node->bitmap_size);
        for (int i = 0; i < lookup_table_len; ++i){
            Lookup_Table * temp_lookup_table = now_node -> lookup_table[i];

            while (temp_lookup_table != NULL)
            {
                if( temp_lookup_table -> pbTrie_node != NULL){
                    get_pbTrie_status(temp_lookup_table -> pbTrie_node, level_number + 1);
                }

                temp_lookup_table = temp_lookup_table -> down_lookup_table;
            }
        }
    }
    return;
}

void print_pbTrie_node_name(PBTrie_node * input_node){
    if (input_node != NULL) {
        printf("rule id : %d\t, branch counter : %d\t, bitmap_size : %d\t, node name : %s\n", input_node -> rule_id, input_node -> branch_couter, input_node -> bitmap_size, input_node -> node_name);
    }
}

void DFS_print_pbTrie(PBTrie_node * input_node){
    


    for (int i = 0; i < accumulate_i_of_bitmap(input_node -> bitmap, input_node -> bitmap_size); ++i){
        if(input_node -> lookup_table[i] != NULL){
            Lookup_Table * temp_lookup_table = input_node -> lookup_table[i];

            while(temp_lookup_table != NULL){
                DFS_print_pbTrie(temp_lookup_table -> pbTrie_node);
                temp_lookup_table = temp_lookup_table -> down_lookup_table;
            }
        }
    }

    print_pbTrie_node_name(input_node);
}

void init_pbTrie_root(){
    pbTrie_root = (PBTrie_node *)malloc(sizeof(PBTrie_node));
    pbTrie_root -> bitmap = NULL;
    pbTrie_root -> node_name[0] = 0;
    pbTrie_root -> rule_id = 0;
    pbTrie_root -> branch_couter = 0;
    pbTrie_root -> bitmap_size = 0;
    pbTrie_root -> lookup_table = NULL;
    pbTrie_root -> father_pbTrie_node = NULL;
}


PBTrie_node * check_pbTrie_exist_by_fingerprint(Lookup_Table * lookup_table, char * strtok_string){
    while(lookup_table != NULL){
        if (strcmp(lookup_table -> pbTrie_node -> node_name, strtok_string) == 0){
            return lookup_table -> pbTrie_node;
        }

        lookup_table = lookup_table -> down_lookup_table;
    }

    return NULL;
}

PBTrie_node * check_branch_exist(PBTrie_node * now_node, char* input_string){
    //printf("in \"check_branch_exist\", MSG : input_string is : %s\n", input_string);

    if(now_node -> bitmap_size == 0){
        // branch not exist
        //printf("in \"check_branch_exist\", MSG : branch not exist, bitmap_size is 0\n");
        return NULL;
    }
    else {
        //maybe branch exist
        //printf("in \"check_branch_exist\", MSG : bitmap_size: %d\n", now_node -> bitmap_size);
        int index_of_bitmap = get_hash_value(input_string, strlen(input_string), now_node -> bitmap_size);
        //printf("in \"check_branch_exist\", MSG : get index of bitmap: %d\n", index_of_bitmap);
        if (now_node -> bitmap[index_of_bitmap] == 0){
            // branch not exist
            return NULL;
        }
        else{
            // maybe branch exist
            int index_of_lookup_table = accumulate_i_of_bitmap(now_node -> bitmap, index_of_bitmap);
            //printf("in \"index_of_lookup_table\", MSG : get index of lookup_table: %d\n", index_of_lookup_table);
            if(now_node -> lookup_table == NULL){
                //branch not exist
                return NULL;
            }
            else{
                //maybe branch exist
                Lookup_Table * temp_lookup_table = now_node -> lookup_table[index_of_lookup_table];

                while(temp_lookup_table != NULL){
                    if(temp_lookup_table -> pbTrie_node != NULL){
                        if( strcmp(input_string, temp_lookup_table -> pbTrie_node -> node_name) == 0 ){
                            //branch exist
                            return temp_lookup_table -> pbTrie_node;
                        }
                        else;
                    }
                    else;
                    temp_lookup_table = temp_lookup_table -> down_lookup_table;
                }
                //branch not exist;
                return NULL;
            }
        }
    }
}

PBTrie_node * create_new_branch(char * input_string){
    PBTrie_node * new_pbTrie_node = create_pbTrie_node();
    strcpy(new_pbTrie_node -> node_name, input_string);

    return new_pbTrie_node;
}

void insert_pbTrie_to_lookup_table_tail(PBTrie_node * new_node, Lookup_Table * lookup_table){
    //printf("in \"insert_pbTrie_to_lookup_table_tail\", MSG : recursively check until find tail\n");
    //recursively check until find tail
    while(lookup_table -> pbTrie_node != NULL){
        if(lookup_table -> down_lookup_table == NULL){
            //need to malloc new lookup table
            lookup_table -> down_lookup_table = create_lookup_table();
            lookup_table = lookup_table -> down_lookup_table;
        }
        else{
            lookup_table = lookup_table -> down_lookup_table;
        }        
    }

    //insert pbTrie node to tail
    lookup_table -> pbTrie_node = new_node;
    //printf("in \"insert_pbTrie_to_lookup_table_tail\", MSG : insert pbTrie node to tail\n");
}

void move_old_lookup_table_to_new_lookup_table(PBTrie_node * new_node, Lookup_Table ** old_lookup_table, int old_bitmap_size, int bitmap_size_change_flag){
    //printf("in \"move_old_lookup_table_to_new_lookup_table\", MSG : old_bitmap_size: %d\n", old_bitmap_size);
    //recursively move old lookup_table's pbTrie
    for(int i = 0; i < old_bitmap_size; ++i){
        Lookup_Table * temp_old_lookup_table = old_lookup_table[i];
        
        while(temp_old_lookup_table != NULL){
            //get new bitmap index of this old lookup_table's pbTrie node in the new pbTrie node
            //printf("in \"move_old_lookup_table_to_new_lookup_table\", MSG : move name: %s\n", temp_old_lookup_table -> pbTrie_node -> node_name);

            int bitmap_index = get_hash_value(temp_old_lookup_table -> pbTrie_node -> node_name, strlen(temp_old_lookup_table -> pbTrie_node -> node_name), new_node -> bitmap_size);
            //printf("in \"move_old_lookup_table_to_new_lookup_table\", MSG : new bitmap index : %d\n", bitmap_index);

            //get new lookup_table index
            int lookup_table_index = accumulate_i_of_bitmap(new_node -> bitmap, bitmap_index);
            //printf("in \"move_old_lookup_table_to_new_lookup_table\", MSG : lookup table index : %d\n", lookup_table_index);
            
            if(new_node->lookup_table[lookup_table_index] == NULL){
                //new lookup_table[lookup_table_index] is null, just move old lookup_table to here
                new_node->lookup_table[lookup_table_index] = temp_old_lookup_table;

                if(bitmap_size_change_flag == 1){
                    //bitmap size was change, you need to move every object of a lookup_table link_list
                    //change into next old lookup_table element
                    temp_old_lookup_table = temp_old_lookup_table -> down_lookup_table;
                    //cut the link list of the moved lookup_table

                    new_node->lookup_table[lookup_table_index] -> down_lookup_table = NULL;
                }
                else{
                    //bitmap size is not change, break to next lookup_table point array
                    break;
                }
                
            }
            else{
                //need to recursively to find the tail to move old lookup_table
                Lookup_Table * temp_new_lookup_table = new_node->lookup_table[lookup_table_index];
                while(temp_new_lookup_table -> down_lookup_table != NULL){
                    temp_new_lookup_table = temp_new_lookup_table -> down_lookup_table;
                }

                //connect old lookup_table to tail
                temp_new_lookup_table -> down_lookup_table = temp_old_lookup_table;

                if(bitmap_size_change_flag == 1){
                    //bitmap size was change, you need to move every object of a lookup_table link_list
                    //change into next old lookup_table element
                    temp_old_lookup_table = temp_old_lookup_table -> down_lookup_table;
                    //cut the link list of the moved lookup_table
                    temp_new_lookup_table -> down_lookup_table -> down_lookup_table = NULL;
                }
                else{
                    //bitmap size is not change, break to next lookup_table point array
                    break;
                }
            }
        }
    }
    // all old lookup_table haved moved, free old lookup_table pointer array
    free(old_lookup_table);

}

void update_old_bitmap_to_new_bitmap(PBTrie_node * new_node, Lookup_Table ** old_lookup_table, int old_accumulation_bitmap){
    //printf("in \"update_old_bitmap_to_new_bitmap\", MSG : old_accumulation_bitmap : %d\n", old_accumulation_bitmap);
    
    for(int i = 0; i < old_accumulation_bitmap; ++i){
        Lookup_Table * temp_old_lookup_table = old_lookup_table[i];
        //printf("in \"update_old_bitmap_to_new_bitmap\", MSG : i : %d\n", i);

        while(temp_old_lookup_table != NULL){
            if (temp_old_lookup_table != NULL){
                if (temp_old_lookup_table -> pbTrie_node != NULL){
                    
                    //get new bitmap index
                    int bitmap_index = get_hash_value(temp_old_lookup_table -> pbTrie_node -> node_name, strlen(temp_old_lookup_table -> pbTrie_node -> node_name), new_node -> bitmap_size);
                    //printf("in \"update_old_bitmap_to_new_bitmap\", MSG : bitmap_index : %d\n", bitmap_index);

                    //update bitmap
                    new_node->bitmap[bitmap_index] = 1;
                    
                }
                else{
                    printf("in \"update_old_bitmap_to_new_bitmap\", ERR : pbTrie node is null\n");
                }
            }
            else;
            
            temp_old_lookup_table = temp_old_lookup_table -> down_lookup_table;
            
        }
        
    }
}

PBTrie_node * insert_pbTrie_node(char* read_string, PBTrie_node * now_node){
    //printf("in \"insert_pbTrie_node\", MSG : check string is null or not\n");

    char input_string[100];
    strcpy(input_string, read_string);

    PBTrie_node * temp_node = now_node;

    //check input string is null or not
    
    if (input_string[0]){
        // input string is not null
        if (input_string[strlen(input_string) - 1] == '\n') {
            //printf("in \"insert_pbTrie_node\", MSG : delete change line\n");
            input_string[strlen(input_string) - 1] = '\0';
        }
        //printf("\nin \"insert_pbTrie_node\", MSG : input message is : %s\n", input_string);
        //check branch exist of not
        
        PBTrie_node * check_branch_exist_or_not_pbTrie_node = check_branch_exist(temp_node, input_string);

        if(check_branch_exist_or_not_pbTrie_node == NULL){
            //printf("in \"insert_pbTrie_node\", MSG : branch not exist\n");
            //branch not exist
            //add branch number
            temp_node -> branch_couter += 1;
            //printf("in \"insert_pbTrie_node\", MSG : add branch_counter: %d\n", temp_node -> branch_couter);
            
            //compare bitsize number
            int old_bitmap_size = temp_node -> bitmap_size;
            temp_node -> bitmap_size = get_pow_value_of_2(temp_node -> branch_couter);
            //printf("in \"insert_pbTrie_node\", MSG : old bitmap_size: %d, new bitmap_size: %d\n", old_bitmap_size, temp_node -> bitmap_size);

            //save old lookup table size
            int old_lookup_table_size = accumulate_i_of_bitmap(temp_node -> bitmap, old_bitmap_size);
            //printf("in \"insert_pbTrie_node\", MSG : old_lookup_table_size: %d\n", old_lookup_table_size);

            //chech need to realloc bitmap and lookup_table or not
            if(old_bitmap_size != temp_node -> bitmap_size){
                //bitmap size different, need to remalloc bitmap and lookup_table
                //printf("in \"insert_pbTrie_node\", MSG : bitmap size different, need to remalloc bitmap and lookup_table\n");

                //create new bitmap
                free(temp_node -> bitmap);
                temp_node -> bitmap = (char * )calloc(temp_node -> bitmap_size, sizeof(char));

                //update_new_bitamp
                update_old_bitmap_to_new_bitmap(temp_node, temp_node -> lookup_table, old_lookup_table_size);
            }
            else; //bitmap size same, no need to remalloc bitmap and lookup_table

            
            
            //get bitmap index
            int bitmap_index =  get_hash_value(input_string, strlen(input_string), temp_node -> bitmap_size);
            //printf("in \"insert_pbTrie_node\", MSG : get bitmap index : %d\n", bitmap_index);
            //upadte bitmap of new branch
            temp_node->bitmap[bitmap_index] = 1;

            //get new lookup table size
            int new_lookup_table_size = accumulate_i_of_bitmap(temp_node -> bitmap, temp_node -> bitmap_size);
            //check lookup table size change or not
            if(old_lookup_table_size != new_lookup_table_size){
                //need to realloc lookup table
                Lookup_Table ** old_lookup_table = temp_node -> lookup_table;
                temp_node -> lookup_table = create_lookup_table_pointer_array(new_lookup_table_size);

                //move old lookup_table to new lookup_table
                int bitmap_size_change_flag = (old_bitmap_size != temp_node -> bitmap_size)? 1:0;
                move_old_lookup_table_to_new_lookup_table(temp_node, old_lookup_table, old_lookup_table_size, bitmap_size_change_flag);
            }   
            else;


            //printf("in \"insert_pbTrie_node\", MSG : start malloc new branch\n");
            //malloc new branch
            PBTrie_node * new_pbTrie_node = create_new_branch(input_string);

            //accumulate bitmap's "1" to get lookup table index
            int lookup_table_index = accumulate_i_of_bitmap(temp_node -> bitmap, bitmap_index);
            //printf("in \"insert_pbTrie_node\", MSG : get lookup table index : %d\n", lookup_table_index);
            //check a lookup table index is null or not
            if (temp_node -> lookup_table[lookup_table_index] == NULL){
                //lookup index is null, need to malloc
                //printf("in \"insert_pbTrie_node\", MSG : lookup index is null, need to malloc\n");
                temp_node -> lookup_table[lookup_table_index] = create_lookup_table();
            }
            else;
            //choose lookup_table to insert pbTrie node
            Lookup_Table * temp_lookup_table = temp_node -> lookup_table[lookup_table_index];
            //insert new pbTrie node to lookup_table's tail
            insert_pbTrie_to_lookup_table_tail(new_pbTrie_node, temp_lookup_table);
            //connect new pbTrie's father pointer
            new_pbTrie_node -> father_pbTrie_node = temp_node;
            //return new pbTrie node
            //printf("in \"insert_pbTrie_node\", MSG : return new pbTrie node\n");
            return new_pbTrie_node;

        }
        else{
            //branch exist
            //just return Pbtrie node
            return check_branch_exist_or_not_pbTrie_node;
        }
    }
    else{
        //input string is null
        printf("in \"insert_pbTrie_node\", ERROR : input string is null\n");
        return now_node;
    }
}

void change_rule_id(PBTrie_node * now_node, int rule_id){
    if(now_node != NULL){
        //printf("in \"change_rule_id\", MSG : change rule id : %d\n", rule_id);
        now_node -> rule_id = rule_id;
    }
    else{
        printf("in \"change_rule_id\", ERR : PBTrie_node is NULL\n");
    }
}

void read_file_and_build_pbTrie(char * file_name){

    printf("in \"read_file_and_build_pbTrie\", MEG : read_file : %s\n", file_name);
    char buf[500];
    char* strtok_string;

    FILE *fptr;
    fptr = fopen(file_name, "r");   

    char tok[] = "/";
    int rule_count = 0;
    while(fgets(buf,500,fptr)!=NULL){
        strtok_string = strtok(buf, tok);
        PBTrie_node * next_pbTrie_node = pbTrie_root;
        while(strtok_string != NULL){
            //printf("%s", strtok_string);
            cycle_begin = rdtsc();
            PBTrie_node * temp_pbTrie_node = insert_pbTrie_node(strtok_string, next_pbTrie_node);
            cycle_end = rdtsc();

            total_build_cycle += (cycle_end - cycle_begin);
            if (max_build_cycle < (cycle_end - cycle_begin))   max_build_cycle = (cycle_end - cycle_begin);
            if (min_build_cycle > (cycle_end - cycle_begin))   min_build_cycle = (cycle_end - cycle_begin);

            next_pbTrie_node = temp_pbTrie_node;
            strtok_string = strtok(NULL, tok);

            if(strtok_string == NULL){
                //this is the end of this prefix, change the rule id
                ++rule_count;
                ++total_build_count;
                change_rule_id(temp_pbTrie_node, rule_count);
            }
        }
        //printf("\n");
        
    }
    
    //close file
    fclose(fptr);

}

PBTrie_node * search_pbTrie_node(char* read_string, PBTrie_node * now_node){
    //printf("in \"search_pbTrie_node\", MSG : check string is null or not\n");

    char input_string[100];
    strcpy(input_string, read_string);

    PBTrie_node * temp_node = now_node;
    if (input_string[0]){
        // input string is not null
        if (input_string[strlen(input_string) - 1] == '\n') {
            //delete change line
            input_string[strlen(input_string) - 1] = '\0';
        }
        //printf("\nin \"search_pbTrie_node\", MSG : input message is : %s\n", input_string);
        //check branch exist of not
        PBTrie_node * check_branch_exist_or_not_pbTrie_node = check_branch_exist(temp_node, input_string);

        if(check_branch_exist_or_not_pbTrie_node == NULL){
            //printf("in \"search_pbTrie_node\", MSG : branch not exist\n");
            //branch not exist
            //return now node
            return temp_node;
        }
        else{
            //branch exist
            //just return Pbtrie node
            temp_node = check_branch_exist_or_not_pbTrie_node;
            return temp_node;
        }

    }
    else{
        //input string is null
        printf("in \"search_pbTrie_node\", ERROR : input string is null\n");
        return now_node;
    }
}

int check_prefix_node(PBTrie_node* temp_pbTrie_node){
    if (temp_pbTrie_node == NULL){
        printf("in \"check_prefix_node\", ERROR : PBTrie_node is null\n");
        return 0;
    }
    else{
        if(temp_pbTrie_node -> rule_id > 0){
            // is prefix node, return rule id 
            return temp_pbTrie_node -> rule_id;
        }
        else{
            //not prefix node, return 0
            return 0;
        }
    }


}

void read_file_and_search_pbTrie(char * file_name){
    //printf("in \"read_file_and_search_pbTrie\", MEG : read_file : %s\n", file_name);
    char buf[500];
    char* strtok_string;

    FILE *fptr;
    fptr = fopen(file_name, "r");

    char tok[] = "/";

    int search_count = 0;

    while(fgets(buf,500,fptr)!=NULL){
        int return_rule_id = 0;

        strtok_string = strtok(buf, tok);
        PBTrie_node * next_pbTrie_node = pbTrie_root;
        // check return pbTrie node is null or not
        while(strtok_string != NULL){
            //printf("%s", strtok_string);
            PBTrie_node * temp_pbTrie_node = search_pbTrie_node(strtok_string, next_pbTrie_node);
            next_pbTrie_node = temp_pbTrie_node;
            strtok_string = strtok(NULL, tok);

            if(temp_pbTrie_node != NULL){
                //return node is not NULL,  check if prefix node or not
                int check_prefix_node_return = check_prefix_node(temp_pbTrie_node);
                if(check_prefix_node_return > 0){
                    //is prefix node, update return_rule_id
                    return_rule_id = check_prefix_node_return;
                }
            }
        
            
        }
        printf("number of '%d' search answer's rule id : %d\n", ++search_count, return_rule_id);
        //printf("\n");
        
    }
    
    //close file
    fclose(fptr);
}

int check_leaf_node(PBTrie_node* now_node){
    if(now_node == NULL){
        printf("in \"check_leaf_node\", ERR : PBTrie_node is NULL\n");
        return -1;
    }
    else{
        if(now_node -> branch_couter <= 0){
            //is leaf node
            return 1;
        }
        else{
            //not node
            return 0;
        }
    }
}

void update_bitmap(PBTrie_node* now_node, int old_lookup_table_pointer_array_size){
    if(now_node == NULL){
        printf("in \"check_need_to_realloc_lookup_table\", ERR : PBTrie_node is NULL\n");

    }
    else{
        for(int i = 0; i < now_node -> bitmap_size; ++i){
            now_node -> bitmap[i] = 0;
        }
        for(int i = 0; i < old_lookup_table_pointer_array_size; ++i){
            if(now_node -> lookup_table[i] != NULL){
                //update bitmap
                int bitmap_index = get_hash_value(now_node -> lookup_table[i] -> pbTrie_node -> node_name, strlen(now_node -> lookup_table[i] -> pbTrie_node -> node_name), now_node -> bitmap_size);
                now_node -> bitmap[bitmap_index] = 1;
            }
        }
    }
}

void recursively_delete_self_and_father_node(PBTrie_node * now_node){
    if(now_node == NULL){
        printf("in \"recursively_delete_self_and_father_node\", ERR : PBTrie_node is NULL\n");
        return;
    }
    else{
        PBTrie_node * temp_node = now_node;
        PBTrie_node * father_node = temp_node -> father_pbTrie_node;

        //remember father's old branch counter

        int father_bitmap_index = get_hash_value(temp_node -> node_name, strlen(temp_node -> node_name), father_node -> bitmap_size);
        
        //check father's bitmap size
        if (father_node -> bitmap_size == 0){
            printf("in \"recursively_delete_self_and_father_node\", ERR : father's bitmap_size is 0\n");
        }
        else{
            int father_lookup_table_index = accumulate_i_of_bitmap(father_node -> bitmap, father_bitmap_index);
            //check father's lookup table
            if (father_node -> lookup_table == NULL){
                printf("in \"recursively_delete_self_and_father_node\", ERR : father's lookup_table is NULL\n");
            }
            else{
                //match node name to get father's lookup_table's pointer

                
                if(father_node -> lookup_table[father_lookup_table_index] -> pbTrie_node != NULL){
                    //need_to_free_lookup_table is at the first entry of a lookup_table pointer array
                    if(strcmp(temp_node -> node_name, father_node -> lookup_table[father_lookup_table_index] -> pbTrie_node -> node_name) == 0 ){
                        //find father's branch need to delete
                        Lookup_Table * need_to_free_lookup_table = father_node -> lookup_table[father_lookup_table_index];
                        father_node -> lookup_table[father_lookup_table_index] = father_node -> lookup_table[father_lookup_table_index] -> down_lookup_table;
                        
                        free(need_to_free_lookup_table);
                        father_node -> branch_couter -= 1;
                    }
                    //need_to_free_lookup_table is not at the first entry of a lookup_table pointer array
                    //need to lookup lookup_table link list
                    else{
                        Lookup_Table * prev_lookup_table = father_node -> lookup_table[father_lookup_table_index];
                        Lookup_Table * temp_lookup_table = prev_lookup_table -> down_lookup_table;

                        while(temp_lookup_table != NULL){
                            if(temp_lookup_table -> pbTrie_node !=NULL){
                                if(strcmp(temp_node -> node_name, temp_lookup_table -> pbTrie_node -> node_name) == 0){
                                    prev_lookup_table -> down_lookup_table = temp_lookup_table -> down_lookup_table;
                                    free(temp_lookup_table);
                                    father_node -> branch_couter -= 1;
                                    break;
                                }

                                prev_lookup_table = temp_lookup_table;
                                temp_lookup_table = temp_lookup_table -> down_lookup_table;
                            }
                            else{
                                printf("in \"recursively_delete_self_and_father_node\", ERR : there is a pbTrie node is NULL\n");
                            }
                        }
                    }
                }
                else{
                    printf("in \"recursively_delete_self_and_father_node\", ERR : there is a pbTrie node is NULL\n");
                }



            }
        }

        //free the pbTrie node
        free(temp_node -> bitmap);
        free(temp_node -> lookup_table);
        free(temp_node);

        //check need to realloc father's lookup_table or not
        int old_father_lookup_table_size = accumulate_i_of_bitmap(father_node -> bitmap, father_node -> bitmap_size);
        int old_father_bitmap_size = father_node -> bitmap_size;

        //update bitmap of father
        update_bitmap(father_node, old_father_lookup_table_size);

        //get new lookup table size
        int new_father_lookup_table_size = accumulate_i_of_bitmap(father_node -> bitmap, father_node -> bitmap_size);
        //check lookup table size change or not
        if(old_father_lookup_table_size != new_father_lookup_table_size){
            //need to realloc lookup table
            Lookup_Table ** old_father_lookup_table = father_node -> lookup_table;
            father_node -> lookup_table = create_lookup_table_pointer_array(new_father_lookup_table_size);

            //move old lookup_table to new lookup_table
            int bitmap_size_change_flag = (old_father_bitmap_size != father_node -> bitmap_size)? 1:0;
            move_old_lookup_table_to_new_lookup_table(father_node, old_father_lookup_table, old_father_lookup_table_size, bitmap_size_change_flag);
        }   
        else;


        //check nned to delete father node or not(beacuse father node is empty)
        int check_leaf_node_or_not = check_leaf_node(father_node);
        int check_not_root = (father_node -> father_pbTrie_node == NULL)? 0:1;
        if(check_leaf_node_or_not == 1 && check_not_root == 1){
            //father is a leaf node, need to delete
            recursively_delete_self_and_father_node(father_node);
        }
    }
}

void delete_pbTrie(PBTrie_node* now_node){
    if(now_node == NULL){
        printf("in \"delete_pbTrie\", ERR : PBTrie_node is NULL\n");
        return;
    }
    else{
        PBTrie_node * temp_node = now_node;
        //check this node is leaf_node or not
        int check_leaf_node_or_not = check_leaf_node(temp_node);
        int check_not_root = (temp_node -> father_pbTrie_node == NULL)? 0:1;
        if(check_leaf_node_or_not == 1 && check_not_root == 1){
            //is leaf node
            //recursively delete this node and father node until the node have other branch or is a prefix node
            recursively_delete_self_and_father_node(now_node);
        }
        else{
            //not a leaf node or is a root, just clean the rule id
            temp_node -> rule_id = 0;
            return;
        }
    }
}

void read_file_and_delete_pbTrie(char * file_name){
    printf("in \"read_file_and_delete_pbTrie\", MEG : read_file : %s\n", file_name);
    char buf[500];
    char* strtok_string;

    FILE *fptr;
    fptr = fopen(file_name, "r");

    char tok[] = "/";

    while(fgets(buf,500,fptr)!=NULL){

        strtok_string = strtok(buf, tok);
        PBTrie_node * next_pbTrie_node = pbTrie_root;
        // check return pbTrie node is null or not
        while(strtok_string != NULL){
            //printf("%s", strtok_string);
            PBTrie_node * temp_pbTrie_node = search_pbTrie_node(strtok_string, next_pbTrie_node);
            next_pbTrie_node = temp_pbTrie_node;
            strtok_string = strtok(NULL, tok);

            if(strtok_string == NULL){
                //it mean search end
                if (temp_pbTrie_node != NULL){
                    //it mean search prefix is match every fingerprint
                    delete_pbTrie(temp_pbTrie_node);
                }
            }
        }
    }
    //close file
    fclose(fptr);
}

void insert_fingerprint(FingerPrint * now_fingerprint, char * input_fingerprint){
    if ( now_fingerprint == NULL){
        printf("in \"now_fingerprint\", ERR : fingerprint is NULL\n");
        return;
    }
    else{
        FingerPrint * temp_fingerprint = now_fingerprint;

        while(temp_fingerprint -> fingerprint_string[0] != 0){
            if(temp_fingerprint -> next_fingerprint == NULL){
                //malloc a new fingerprint 
                temp_fingerprint -> next_fingerprint = create_fingerprint();
                temp_fingerprint = temp_fingerprint -> next_fingerprint;
            }
            else{
                //move to next fingerprint
                temp_fingerprint = temp_fingerprint -> next_fingerprint;
            }
        }
        strcpy(temp_fingerprint -> fingerprint_string, input_fingerprint);

    }
}

void print_fingerprint(FingerPrint * now_fingerprint){
    printf("fingerprint: ");
    while(now_fingerprint != NULL){
        printf(" %s ", now_fingerprint -> fingerprint_string);
        now_fingerprint = now_fingerprint -> next_fingerprint;
    }
    printf("\n");
}

int match_fingerprint(FingerPrint * now_fingerprint, int fingerprint_number, PBC_cache * now_cache){
    //check fingerprint is null or not 
    if(now_fingerprint == NULL){
        printf("in \"match_fingerprint\", ERR : fingerprint is NULL\n");
        return -1;
    }
    //check cache is null or not
    else if (now_cache == NULL){
        printf("in \"match_fingerprint\", ERR : now cache is NULL\n");
        return -1;
    }
    else{
        int match_flag = 1;
        FingerPrint * temp_cache_fingerprint = now_cache -> cache_fingerprint;
        
        if(fingerprint_number > now_cache -> fingerprint_len){
            //input fingerprint is longer than cache
            //not match
            match_flag = 0;
            return 0;
        }

        for(int i = fingerprint_number; i > 0; --i){
            //check fingerprint is same or not
            if(strcmp(now_fingerprint -> fingerprint_string, temp_cache_fingerprint -> fingerprint_string) == 0);
            else{
                //fingerprint is different
                match_flag = 0;
                return match_flag;
            }

            now_fingerprint = now_fingerprint -> next_fingerprint;
            temp_cache_fingerprint = temp_cache_fingerprint -> next_fingerprint;
        }

        return match_flag;
    }
}

PBC_cache * search_hash_table_index(FingerPrint * now_fingerprint, int fingerprint_number, Hash_Table * now_hash_table){
    //printf("in \"search_hash_table_index\", MSG : fingerprint_number : %d\n", fingerprint_number);
    
    //check fingerprint is null or not 
    if(now_fingerprint == NULL){
        printf("in \"search_hash_table_index\", ERR : fingerprint is NULL\n");
        return NULL;
    }
    //check hash table is null or not
    if (now_hash_table == NULL){
        return NULL;
    }
    else{
        Hash_Table * temp_hash_table = now_hash_table;
        while(temp_hash_table != NULL){
            //try to match a cache's fingerprint
            //printf(" node name %s ", temp_hash_table -> cache_pointer -> pbTrie_pointer -> node_name);
            int match_flag = match_fingerprint(now_fingerprint, fingerprint_number, temp_hash_table -> cache_pointer);
            if(match_flag == 1){
                //find cache
                return temp_hash_table -> cache_pointer;
            }
            temp_hash_table = temp_hash_table -> down_hash_table;
        }

        //not fount any match cache
        return NULL;
        
    }
}

int get_hash_table_index(FingerPrint * now_fingerprint, int fingerprint_number){
    int hash_table_index = 0;

    for(int i = 0; i < fingerprint_number; ++i){
            hash_table_index += get_hash_value(now_fingerprint -> fingerprint_string, strlen(now_fingerprint -> fingerprint_string), HASH_TABLE_SIZE);
            now_fingerprint = now_fingerprint -> next_fingerprint;
        }
    hash_table_index = hash_table_index % HASH_TABLE_SIZE;

    return hash_table_index;
}

PBC_cache * search_hash_table_with_fingerprint(FingerPrint * now_fingerprint, int fingerprint_number){
    //printf("in \"search_hash_table_with_fingerprint\", MSG : fingerprint_number : %d\n", fingerprint_number);
    
    //check fingerprint is null or not 
    if(now_fingerprint == NULL){
        printf("in \"search_hash_table_with_fingerprint\", ERR : fingerprint is NULL\n");
        return NULL;
    }
    else{
        //get hash table index
        int hash_table_index = get_hash_table_index(now_fingerprint, fingerprint_number);
        //printf("in \"search_hash_table_with_fingerprint\", MSG : hash_table_index : %d\n", hash_table_index);

        //searh hash table
        PBC_cache * answer_cache = search_hash_table_index(now_fingerprint, fingerprint_number, hash_table[hash_table_index]);
        //printf("in \"search_hash_table_with_fingerprint\", MSG : after search_hash_table_index\n");

        if (answer_cache != NULL){
            //find a cache
            return answer_cache;
        }
        else{
            // cache not found
            return NULL;
        }
    }
}

PBC_cache * get_cache_tail(){
    PBC_cache * temp_PBC_cache = PBC_cache_head;

    if(temp_PBC_cache == NULL){
        printf("in \"get_cache_tail\", ERR : cache head is NULL\n");
        return NULL;
    }
    //recursively to get cache tail
    while(temp_PBC_cache -> down_PBC_cache != NULL){
        temp_PBC_cache = temp_PBC_cache -> down_PBC_cache;
    }

    return temp_PBC_cache;
}

void remove_hash_entry(PBC_cache * now_cache){
    if(hash_table == NULL){
        printf("in \"remove_hash_entry\", ERR : hashtable is NULL\n");
        return;
    }
    else if (now_cache == NULL){
        printf("in \"remove_hash_entry\", ERR : now_cache is NULL\n");
        return;
    }
    else{
        //get hash table index
        int hash_table_index = get_hash_table_index(now_cache -> cache_fingerprint, now_cache -> fingerprint_len);

        if(hash_table[hash_table_index] != NULL){
            if(hash_table[hash_table_index] -> cache_pointer != NULL){
                //printf("in \"remove_hash_entry\", MSG : hash_table[i] -> cache_pointer -> rule_id : %d\n", hash_table[i] -> cache_pointer -> rule_id);
                if(now_cache -> rule_id == hash_table[hash_table_index] -> cache_pointer -> rule_id){
                    //delete hash entry
                    Hash_Table * removed_hash_table = hash_table[hash_table_index];
                    hash_table[hash_table_index] = hash_table[hash_table_index] -> down_hash_table;
                    free(removed_hash_table);
                    return;
                }
            }
            else{
                printf("in \"remove_hash_entry\", ERR : cache_pointer is NULL\n");
                return;
            }

            Hash_Table * temp_hash_table = hash_table[hash_table_index] -> down_hash_table;

            while(temp_hash_table != NULL){
                if(temp_hash_table -> cache_pointer != NULL){
                    if(now_cache -> rule_id == temp_hash_table -> cache_pointer -> rule_id){
                        //delete hash entry
                        Hash_Table * removed_hash_table = temp_hash_table;
                        temp_hash_table = temp_hash_table -> down_hash_table;
                        free(removed_hash_table);
                        return;
                    }
                }
                else{
                    printf("in \"remove_hash_entry\", ERR : cache_pointer is NULL\n");
                    return;
                }

                //check down hash entry
                temp_hash_table = temp_hash_table -> down_hash_table;
            }
        }
        
    }

    //printf("in \"remove_hash_entry\", MSG : rule_id : %d\n", rule_id);

    
}

void free_fingerprint_recursively(FingerPrint * now_fingerprint){
    if(now_fingerprint != NULL){
        free_fingerprint_recursively(now_fingerprint -> next_fingerprint);
    }

    free(now_fingerprint);
}

void cut_fingerprint(FingerPrint * now_fingerprint, int fingerprint_count){
    if(now_fingerprint == NULL){
        printf("in \"cut_fingerprint\", ERR : now_fingerprint is NULL\n");
        return;
    }
    else{
        //cut fingerprint at fingerprint_count
        FingerPrint * temp_fingerprint = now_fingerprint;
        FingerPrint * pre_fingerprint = NULL;

        for(int i = 0; i < fingerprint_count; ++i){
            pre_fingerprint = temp_fingerprint;
            temp_fingerprint = temp_fingerprint -> next_fingerprint;
        }

        //cut fingerprint
        pre_fingerprint -> next_fingerprint = NULL;
        free_fingerprint_recursively(temp_fingerprint);

    }
}

void insert_to_hash_table(PBC_cache * new_cache, int hash_table_index){
    if(new_cache == NULL){
        printf("in \"insert_to_hash_table\", ERR : new_cache is NULL\n");
        return;
    }
    else{
        if(hash_table[hash_table_index] == NULL){
            hash_table[hash_table_index] = create_hash_table();
            hash_table[hash_table_index] -> cache_pointer = new_cache;
            return;
        }
        else{
            Hash_Table * new_hash_table = create_hash_table();
            new_hash_table -> cache_pointer = new_cache;

            Hash_Table *temp_hash_table = hash_table[hash_table_index];
            while (temp_hash_table -> down_hash_table != NULL){
                temp_hash_table = temp_hash_table -> down_hash_table;
            }
            temp_hash_table -> down_hash_table = new_hash_table;
            return;
        }
    }
}

int PBC_search(FingerPrint * now_fingerprint, int fingerprint_number){
    //printf("in \"PBC_search\", MSG : fingerprint_number : %d\n", fingerprint_number);
    //check fingerprint is null or not 
    if(now_fingerprint == NULL){
        printf("in \"PBC_search\", ERR : fingerprint is NULL\n");
        return -1;
    }
    else{
        //match fingerprint from long to short
        //printf("in \"PBC_search\", MSG : match fingerprint from long to short\n");
        int cache_fingerprint_num = 0;
        PBC_cache * answer_cache = NULL;
        for (int i = fingerprint_number; i > 0; i--)
        {
            //search hash with fingerprint
            answer_cache = search_hash_table_with_fingerprint(now_fingerprint, i);

            if(answer_cache != NULL){
                cache_fingerprint_num = i;
                break;
            }
        }

        //printf("in \"PBC_search\", MSG : cache_fingerprint_num : %d\n", cache_fingerprint_num);

        if(answer_cache != NULL){
            //it mean we fine a cache in all cache
            //printf("in \"PBC_search\", MSG : we fine a cache in all cache\n");

            //check leaf flag
            if(answer_cache -> pbTrie_pointer -> branch_couter <= 0){
                //is leaf node, just return rule id
                return answer_cache -> rule_id;
            }
            else;
            //check fingerprint len
            if(fingerprint_number == cache_fingerprint_num){
                //is longer match prefix , just retrun rule id
                return answer_cache -> rule_id;
            }
            else;

            //move this cache to cache list's head
            if(answer_cache -> pre_PBC_cahce != NULL){
                answer_cache -> pre_PBC_cahce -> down_PBC_cache = answer_cache -> down_PBC_cache;
            }
            else;
            if(answer_cache -> down_PBC_cache != NULL){
                answer_cache ->down_PBC_cache -> pre_PBC_cahce = answer_cache -> pre_PBC_cahce;
            }
            else;
            /*
            PBC_cache * temp_cache_pointer = PBC_cache_head;
            temp_cache_pointer -> pre_PBC_cahce = answer_cache;
            answer_cache -> down_PBC_cache = temp_cache_pointer;
            PBC_cache_head = answer_cache;
            */

            //check bitmap
            FingerPrint * temp_fingerprint = now_fingerprint;
            //get next finger print
            for(int i = 0; i < fingerprint_number; ++i){
                temp_fingerprint = temp_fingerprint -> next_fingerprint;

                if(temp_fingerprint == NULL){
                    printf("in \"PBC_search\", ERR : next fingerprint is NULL\n");
                }
            }
            int bitmap_index = get_hash_value(temp_fingerprint -> fingerprint_string, strlen(temp_fingerprint -> fingerprint_string), answer_cache -> pbTrie_pointer -> bitmap_size);

            if(answer_cache -> pbTrie_pointer -> bitmap[bitmap_index] == 0){
                //longest prefix match, return rule id 
                return answer_cache -> rule_id;
            }

            //search pbTrie node from immediate node
            //printf("in \"PBC_search\", MSG : search pbTrie node from immediate node\n");
            int answer_rule_id = answer_cache -> rule_id;
            PBTrie_node * temp_pbTrie_node = answer_cache -> pbTrie_pointer;
            while(temp_fingerprint != NULL){
                PBTrie_node * next_pbTrie_node = search_pbTrie_node(temp_fingerprint -> fingerprint_string, temp_pbTrie_node);
                temp_pbTrie_node = next_pbTrie_node;

                if(temp_pbTrie_node != NULL){
                    //return node is not NULL,  check if prefix node or not
                    int check_prefix_node_return = check_prefix_node(temp_pbTrie_node);
                    if(check_prefix_node_return > 0){
                        //is prefix node, update return_rule_id
                        answer_rule_id = check_prefix_node_return;
                    }
                }
                temp_fingerprint = temp_fingerprint -> next_fingerprint;
            }

            //free fingerprint
            //printf("in \"PBC_search\", MSG : free fingerprint\n");
            free_fingerprint_recursively(now_fingerprint);

            //return rule id
            return answer_rule_id;
        }
        else{
            //printf("in \"PBC_search\", MSG : not in the cache, search from pbTrei root, need to insert to cache\n");
            //not in the cache, search from pbTrei root, need to insert to cache 
            FingerPrint * temp_fingerprint = now_fingerprint;
            int answer_rule_id = 0;
            int fingerprint_temp_count = 0;
            int fingerprint_final_count = 0;
            PBTrie_node * next_pbTrie_node = pbTrie_root;
            PBTrie_node * longest_prefix_node = NULL;
            // check return pbTrie node is null or not
            while(temp_fingerprint != NULL){
                PBTrie_node * temp_pbTrie_node = search_pbTrie_node(temp_fingerprint -> fingerprint_string, next_pbTrie_node);
                next_pbTrie_node = temp_pbTrie_node;

                if(temp_pbTrie_node != NULL){
                    ++fingerprint_temp_count;

                    //return node is not NULL,  check if prefix node or not
                    int check_prefix_node_return = check_prefix_node(temp_pbTrie_node);
                    if(check_prefix_node_return > 0){
                        //is prefix node, update answer_rule_id
                        answer_rule_id = check_prefix_node_return;
                        longest_prefix_node = temp_pbTrie_node;
                        //update fingerprint final count
                        fingerprint_final_count = fingerprint_temp_count;
                    }
                }
            
                temp_fingerprint = temp_fingerprint -> next_fingerprint;
            }

            //printf("in \"PBC_search\", MSG : answer_rule_id : %d\n", answer_rule_id);

            if (answer_rule_id != 0){
                //it mean we get prefix
                //insert to cache

                //check cache num is full or not
                //printf("in \"PBC_search\", MSG : now_cache_number : %d\n", now_cache_number);
                if(now_cache_number >= MAX_CACHE_NUM){
                    //delete tail cache or not;

                    //get cache tail
                    PBC_cache * cache_tail = get_cache_tail();
                    if(cache_tail == NULL){
                        printf("in \"PBC_search\", ERR : next cache tail is NULL\n");
                    }

                    //delete the hash entry of this cache
                    remove_hash_entry(cache_tail);
                    

                    //check tail is head or not
                    if(cache_tail -> pre_PBC_cahce == NULL){
                        //clean tail
                        //free fingerprint
                        free_fingerprint_recursively(cache_tail -> cache_fingerprint);
                        cache_tail -> cache_fingerprint = NULL;
                        cache_tail -> rule_id = 0;
                        cache_tail -> pbTrie_pointer = NULL;
                        cache_tail -> pre_PBC_cahce = NULL;
                        cache_tail -> down_PBC_cache = NULL;
                    }
                    else{
                        //delete tail
                        //free fingerprint
                        free_fingerprint_recursively(cache_tail -> cache_fingerprint);
                        cache_tail -> pre_PBC_cahce -> down_PBC_cache = NULL;
                        free(cache_tail);
                    }
                    --now_cache_number;
                }
                else;

                //insert to cache
                PBC_cache * new_PBC_cache = NULL;
                if(now_cache_number == 0){
                    //update head
                    new_PBC_cache = PBC_cache_head;

                    new_PBC_cache -> rule_id = longest_prefix_node -> rule_id;
                    new_PBC_cache -> fingerprint_len = fingerprint_final_count;
                    new_PBC_cache -> pbTrie_pointer = longest_prefix_node;

                    //copy fingerprint
                    cut_fingerprint(now_fingerprint, fingerprint_final_count);
                    new_PBC_cache -> cache_fingerprint = now_fingerprint;

                    int hash_table_index = get_hash_table_index(now_fingerprint, fingerprint_final_count);
                    insert_to_hash_table(new_PBC_cache, hash_table_index);
                }
                else{
                    //create new cache
                    PBC_cache * new_PBC_cache = create_PBC_cache();

                    new_PBC_cache -> rule_id = longest_prefix_node -> rule_id;
                    new_PBC_cache -> fingerprint_len = fingerprint_final_count;
                    new_PBC_cache -> pbTrie_pointer = longest_prefix_node;

                    
                    //connect cache
                    new_PBC_cache -> down_PBC_cache = PBC_cache_head;
                    PBC_cache_head -> pre_PBC_cahce = new_PBC_cache;
                    PBC_cache_head = new_PBC_cache;

                    //copy fingerprint
                    cut_fingerprint(now_fingerprint, fingerprint_final_count);
                    new_PBC_cache -> cache_fingerprint = now_fingerprint;

                    int hash_table_index = get_hash_table_index(now_fingerprint, fingerprint_final_count);
                    insert_to_hash_table(new_PBC_cache, hash_table_index);
                }
            

                ++now_cache_number;

            }
            //retrun rule id
            return answer_rule_id;
        }
        
    }
}

void read_file_and_search_with_cache(char * file_name){
    printf("in \"read_file_and_build_pbTrie\", MEG : read_file : %s\n", file_name);
    char buf[500];
    char* strtok_string;

    FILE *fptr;
    fptr = fopen(file_name, "r");   

    int search_count = 0;

    char tok[] = "/";
    while(fgets(buf,500,fptr)!=NULL){
        strtok_string = strtok(buf, tok);
        FingerPrint * new_fingerprint = create_fingerprint();
        int fingerprint_number = 0;
        // check return pbTrie node is null or not
        while(strtok_string != NULL){
            //printf("%s", strtok_string);
            insert_fingerprint(new_fingerprint, strtok_string);
            ++fingerprint_number;
            strtok_string = strtok(NULL, tok);

        }
        print_fingerprint(new_fingerprint);

        //search cache
        cycle_begin = rdtsc();
        int answer_rule_id = PBC_search(new_fingerprint, fingerprint_number);
        cycle_end = rdtsc();

        total_search_cycle += cycle_end - cycle_begin;
        if (max_search_cycle < (cycle_end - cycle_begin))   max_search_cycle = (cycle_end - cycle_begin);
        if (min_search_cycle > (cycle_end - cycle_begin))   min_search_cycle = (cycle_end - cycle_begin);

        total_search_count += 1;
        printf("number of '%d' search answer's rule id : %d\n", ++search_count, answer_rule_id);
    }
    
    //close file
    fclose(fptr);
}

void PBC_insert(FingerPrint * now_fingerprint, int fingerprint_number, int rule_id){
    //printf("in \"PBC_insert\", MSG : fingerprint_number : %d\n", fingerprint_number);
    //check fingerprint is null or not 
    if(now_fingerprint == NULL){
        printf("in \"PBC_insert\", ERR : fingerprint is NULL\n");
        return;
    }
    else{
        //match fingerprint from long to short
        int cache_fingerprint_num = 0;
        PBC_cache * answer_cache = NULL;
        for (int i = fingerprint_number; i > 0; i--)
        {
            //search hash with fingerprint
            answer_cache = search_hash_table_with_fingerprint(now_fingerprint, i);

            if(answer_cache != NULL){
                cache_fingerprint_num = i;
                break;
            }
        }

        //printf("in \"PBC_insert\", MSG : cache_fingerprint_num : %d\n", cache_fingerprint_num);

        if(answer_cache != NULL){
            //we find cache in cache list
            //printf("in \"PBC_insert\", MSG : we find cache in cache list\n");
            PBTrie_node * next_pbTrie_node = answer_cache -> pbTrie_pointer;
            if(next_pbTrie_node == NULL){
                printf("in \"PBC_insert\", ERR : pbTrie_pointer is NULL\n");
            }

            //move to the fingerprint need to insert
            FingerPrint * temp_fingerprint = now_fingerprint;
            for(int i = 0; i < cache_fingerprint_num; ++i){
                temp_fingerprint = temp_fingerprint -> next_fingerprint;
            }

            //insert the fingerprint to the cache's pbTrie node
            while(temp_fingerprint != NULL){
                //printf("%s", strtok_string);
                PBTrie_node * temp_pbTrie_node = insert_pbTrie_node(temp_fingerprint -> fingerprint_string, next_pbTrie_node);
                next_pbTrie_node = temp_pbTrie_node;
                temp_fingerprint = temp_fingerprint -> next_fingerprint;

                if(temp_fingerprint == NULL){
                    //this is the end of this prefix, change the rule id
                    change_rule_id(temp_pbTrie_node, rule_id);
                }
            }

            //free fingerprint
            free_fingerprint_recursively(now_fingerprint);
        }
        else{
            //printf("in \"PBC_insert\", MSG : not fount cache, insert from root\n");
            //not fount cache, insert from root
            FingerPrint * temp_fingerprint = now_fingerprint;

            PBTrie_node * next_pbTrie_node = pbTrie_root;


            //insert the fingerprint to the pbTrie root
            while(temp_fingerprint != NULL){
                //printf("%s", strtok_string);
                PBTrie_node * temp_pbTrie_node = insert_pbTrie_node(temp_fingerprint -> fingerprint_string, next_pbTrie_node);
                next_pbTrie_node = temp_pbTrie_node;
                temp_fingerprint = temp_fingerprint -> next_fingerprint;

                if(temp_fingerprint == NULL){
                    //this is the end of this prefix, change the rule id
                    change_rule_id(temp_pbTrie_node, rule_id);
                }
            }

            //free fingerprint
            free_fingerprint_recursively(now_fingerprint);
        }
    }
}

void read_file_and_insert_with_cache(char * file_name){
    printf("in \"read_file_and_insert_with_cache\", MEG : read_file : %s\n", file_name);
    char buf[500];
    char* strtok_string;

    FILE *fptr;
    fptr = fopen(file_name, "r");   

    int insert_count = 0;

    char tok[] = "/";
    while(fgets(buf,500,fptr)!=NULL){
        strtok_string = strtok(buf, tok);
        FingerPrint * new_fingerprint = create_fingerprint();
        int fingerprint_number = 0;
        // check return pbTrie node is null or not
        while(strtok_string != NULL){
            //printf("%s", strtok_string);
            insert_fingerprint(new_fingerprint, strtok_string);
            ++fingerprint_number;
            strtok_string = strtok(NULL, tok);

        }
        print_fingerprint(new_fingerprint);

        //search cache
        cycle_begin = rdtsc();
        PBC_insert(new_fingerprint, fingerprint_number, ++insert_count);
        cycle_end = rdtsc();
        total_insert_cycle += cycle_end - cycle_begin;
        if (max_insert_cycle < (cycle_end - cycle_begin))   max_insert_cycle = (cycle_end - cycle_begin);
        if (min_insert_cycle > (cycle_end - cycle_begin))   min_insert_cycle = (cycle_end - cycle_begin);

        total_insert_count += 1;
        printf("number of '%d' insert count\n", insert_count);
    }
    
    //close file
    fclose(fptr);
}

void PBC_delete(FingerPrint * now_fingerprint, int fingerprint_number){
    //printf("in \"PBC_delete\", MSG : fingerprint_number : %d\n", fingerprint_number);
    //check fingerprint is null or not 
    if(now_fingerprint == NULL){
        printf("in \"PBC_delete\", ERR : fingerprint is NULL\n");
        return;
    }
    else{
        //match fingerprint from long to short
        int cache_fingerprint_num = 0;
        PBC_cache * answer_cache = NULL;
        for (int i = fingerprint_number; i > 0; i--)
        {
            //search hash with fingerprint
            answer_cache = search_hash_table_with_fingerprint(now_fingerprint, i);

            if(answer_cache != NULL){
                cache_fingerprint_num = i;
                break;
            }
        }

        //printf("in \"PBC_delete\", MSG : cache_fingerprint_num : %d\n", cache_fingerprint_num);

        if(answer_cache != NULL){
            //we find cache in cache list
           // printf("in \"PBC_delete\", MSG : we find cache in cache list\n");
            PBTrie_node * next_pbTrie_node = answer_cache -> pbTrie_pointer;
            if(next_pbTrie_node == NULL){
                printf("in \"PBC_insert\", ERR : pbTrie_pointer is NULL\n");
            }

            //move to the fingerprint need to start delete
            FingerPrint * temp_fingerprint = now_fingerprint;
            for(int i = 0; i < cache_fingerprint_num; ++i){
                temp_fingerprint = temp_fingerprint -> next_fingerprint;
            }

            //search the delete pbTrie node from fingerprint
            while(temp_fingerprint != NULL){
                //printf("%s", strtok_string);
                PBTrie_node * temp_pbTrie_node = search_pbTrie_node(temp_fingerprint -> fingerprint_string, next_pbTrie_node);
                next_pbTrie_node = temp_pbTrie_node;
                temp_fingerprint = temp_fingerprint -> next_fingerprint;

                if(temp_fingerprint == NULL){
                    //it mean search end
                    if (temp_pbTrie_node != NULL){
                        //it mean search prefix is match every fingerprint
                        delete_pbTrie(temp_pbTrie_node);
                    }
                }
            }

            //if cache is all match the delete pbTrie node, we need to delete hash and cache
            if(fingerprint_number == cache_fingerprint_num){
                //delete the hash entry of this cache
                remove_hash_entry(answer_cache);

                //check tail is head or not
                if(answer_cache -> pre_PBC_cahce == NULL){
                    //clean cache
                    //free fingerprint
                    free_fingerprint_recursively(answer_cache -> cache_fingerprint);
                    answer_cache -> cache_fingerprint = NULL;
                    answer_cache -> rule_id = 0;
                    answer_cache -> pbTrie_pointer = NULL;
                    answer_cache -> pre_PBC_cahce = NULL;

                    //connect other cache
                    if(answer_cache -> down_PBC_cache != NULL){
                        answer_cache -> down_PBC_cache -> pre_PBC_cahce = NULL;
                    }
                }
                else{
                    //delete cache
                    //free fingerprint
                    free_fingerprint_recursively(answer_cache -> cache_fingerprint);

                    //connect other cache
                    if(answer_cache -> down_PBC_cache != NULL){
                        answer_cache -> pre_PBC_cahce -> down_PBC_cache = answer_cache -> down_PBC_cache;
                        answer_cache -> down_PBC_cache -> pre_PBC_cahce = answer_cache -> pre_PBC_cahce;
                    }
                    else{
                        answer_cache -> pre_PBC_cahce -> down_PBC_cache = NULL;
                    }
                    
                    free(answer_cache);
                }
                --now_cache_number;
            }

            //free fingerprint
            free_fingerprint_recursively(now_fingerprint);
        }
        else{
            //not found cache, search delete node from root
            //printf("in \"PBC_delete\", MSG : not fount cache, delete from root\n");
            FingerPrint * temp_fingerprint = now_fingerprint;

            PBTrie_node * next_pbTrie_node = pbTrie_root;


            //delete the fingerprint from the pbTrie root
            while(temp_fingerprint != NULL){
                //printf("%s", strtok_string);
                PBTrie_node * temp_pbTrie_node = search_pbTrie_node(temp_fingerprint -> fingerprint_string, next_pbTrie_node);
                next_pbTrie_node = temp_pbTrie_node;
                temp_fingerprint = temp_fingerprint -> next_fingerprint;

                if(temp_fingerprint == NULL){
                    //it mean search end
                    if (temp_pbTrie_node != NULL){
                        //it mean search prefix is match every fingerprint
                        delete_pbTrie(temp_pbTrie_node);
                    }
                }
            }

            //free fingerprint
            free_fingerprint_recursively(now_fingerprint);
        }
    }
}

void read_file_and_delete_with_cache(char * file_name){
    printf("in \"read_file_and_delete_with_cache\", MEG : read_file : %s\n", file_name);
    char buf[500];
    char* strtok_string;

    FILE *fptr;
    fptr = fopen(file_name, "r");   

    int delete_count = 0;

    char tok[] = "/";
    while(fgets(buf,500,fptr)!=NULL){
        strtok_string = strtok(buf, tok);
        FingerPrint * new_fingerprint = create_fingerprint();
        int fingerprint_number = 0;
        // check return pbTrie node is null or not
        while(strtok_string != NULL){
            //printf("%s", strtok_string);
            insert_fingerprint(new_fingerprint, strtok_string);
            ++fingerprint_number;
            strtok_string = strtok(NULL, tok);

        }
        print_fingerprint(new_fingerprint);

        //search cache
        cycle_begin = rdtsc();
        PBC_delete(new_fingerprint, fingerprint_number);
        cycle_end = rdtsc();
        
        total_delete_cycle += cycle_end - cycle_begin;
        if (max_delete_cycle < (cycle_end - cycle_begin))   max_delete_cycle = (cycle_end - cycle_begin);
        if (min_delete_cycle > (cycle_end - cycle_begin))   min_delete_cycle = (cycle_end - cycle_begin);

        total_delete_count += 1;
        printf("number of '%d' delete count\n", ++delete_count);
    }
    
    //close file
    fclose(fptr);
}


int main(int argc, char* argv[]){
    init_pbTrie_root();
    init_PBC_cache_header();
    init_hash_table();

    if (argc >= 2){
        //build
        read_file_and_build_pbTrie(argv[1]);
        printf("get pbTrie status\n");
        get_pbTrie_status(pbTrie_root, 0);
        //DFS_print_pbTrie(pbTrie_root);
    }
    if(argc >= 3){
        //search
        //read_file_and_search_pbTrie(argv[1]);
        read_file_and_search_with_cache(argv[2]);
    }
    if(argc >= 4){
        //delete
        //read_file_and_delete_pbTrie(argv[3]);
        read_file_and_delete_with_cache(argv[3]);
        //DFS_print_pbTrie(pbTrie_root);
    }
    if(argc >= 5){
        //update
        read_file_and_insert_with_cache(argv[4]);
    }

    printf("total build cycle : %llu\n", total_build_cycle);
    printf("total build count : %llu\n", total_build_count);
    printf("average build cycle : %llu\n", total_build_cycle / total_build_count);
    printf("min build cycle : %llu, max build cycle : %llu\n", min_build_cycle, max_build_cycle);
    printf("total search cycle : %llu\n", total_search_cycle);
    printf("total search count : %llu\n", total_search_count);
    printf("average search cycle : %llu\n", total_search_cycle / total_search_count);
    printf("min search cycle : %llu, max search cycle : %llu\n", min_search_cycle, max_search_cycle);
    printf("total delete cycle : %llu\n", total_delete_cycle);
    printf("total delete count : %llu\n", total_delete_count);
    printf("average delete cycle : %llu\n", total_delete_cycle / total_delete_count);
    printf("min delete cycle : %llu, max delete cycle : %llu\n", min_delete_cycle, max_delete_cycle);
    printf("total insert cycle : %llu\n", total_insert_cycle);
    printf("total insert count : %llu\n", total_insert_count);
    printf("average insert cycle : %llu\n", total_insert_cycle / total_insert_count);
    printf("min insert cycle : %llu, max insert cycle : %llu\n", min_insert_cycle, max_insert_cycle);

    printf("number of pbTire node : %d\n", number_of_pbTrie_node);
    printf("max branch counter : %d\n", max_branch_counter);
    printf("average branch counter : %f\n", (float)branch_counter_counter/(float)number_of_pbTrie_node);
    printf("average bitmap size : %f\n", (float)bitmap_size_counter/(float)number_of_pbTrie_node);
    for(int i = 0; i < 10; ++i){
        printf("number of level %d pbTrie node : %d\n", i, number_of_level_pbTire_node[i]);
    }


    
    
    
    

    //printf(" 5. BKDR-Hash Function Value: %u\n",BKDRHash("google",1));


    //printf("argc : %d\n", argc);

    return 0;
}