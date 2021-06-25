#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"pbTrie.h"

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
int get_bitmap_index(const char* str, unsigned int string_len, int bitmap_size){
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
    printf("in \"create_lookup_table_pointer_array\", MSG : array_len is : %d\n", array_len);
    Lookup_Table ** new_lookup_table = (Lookup_Table **)malloc(array_len * sizeof(Lookup_Table *));
    for(int i = 0; i < array_len; ++i){
        new_lookup_table[i] = NULL;
    }

    return new_lookup_table;
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
        int index_of_bitmap = get_bitmap_index(input_string, strlen(input_string), now_node -> bitmap_size);
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
    printf("in \"insert_pbTrie_to_lookup_table_tail\", MSG : recursively check until find tail\n");
    //recursively check until find tail
    while(lookup_table -> pbTrie_node != NULL){
        if(lookup_table -> down_lookup_table == NULL){
            //need to malloc new lookup table
            lookup_table -> down_lookup_table = create_lookup_table();
        }
        else;

        lookup_table = lookup_table -> down_lookup_table;
    }

    //insert pbTrie node to tail
    lookup_table -> pbTrie_node = new_node;
    printf("in \"insert_pbTrie_to_lookup_table_tail\", MSG : insert pbTrie node to tail\n");
}

void move_old_lookup_table_to_new_lookup_table(PBTrie_node * new_node, Lookup_Table ** old_lookup_table, int old_bitmap_size, int bitmap_size_change_flag){
    printf("in \"move_old_lookup_table_to_new_lookup_table_and_update_bitmap\", MSG : old_bitmap_size: %d\n", old_bitmap_size);
    //recursively move old lookup_table's pbTrie
    for(int i = 0; i < old_bitmap_size; ++i){
        Lookup_Table * temp_old_lookup_table = old_lookup_table[i];
        
        while(temp_old_lookup_table != NULL){
            //get new bitmap index of this old lookup_table's pbTrie node in the new pbTrie node
            printf("in \"move_old_lookup_table_to_new_lookup_table_and_update_bitmap\", MSG : move name: %s\n", temp_old_lookup_table -> pbTrie_node -> node_name);

            int bitmap_index = get_bitmap_index(temp_old_lookup_table -> pbTrie_node -> node_name, strlen(temp_old_lookup_table -> pbTrie_node -> node_name), new_node -> bitmap_size);
            printf("in \"move_old_lookup_table_to_new_lookup_table_and_update_bitmap\", MSG : new bitmap index : %d\n", bitmap_index);

            //get new lookup_table index
            int lookup_table_index = accumulate_i_of_bitmap(new_node -> bitmap, bitmap_index);
            printf("in \"move_old_lookup_table_to_new_lookup_table_and_update_bitmap\", MSG : lookup table index : %d\n", lookup_table_index);
            
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
    for(int i = 0; i < old_accumulation_bitmap; ++i){
        Lookup_Table * temp_old_lookup_table = old_lookup_table[i];

        while(temp_old_lookup_table != NULL){
            if (temp_old_lookup_table -> pbTrie_node != NULL){

                //get new bitmap index
                int bitmap_index = get_bitmap_index(temp_old_lookup_table -> pbTrie_node -> node_name, strlen(temp_old_lookup_table -> pbTrie_node -> node_name), new_node -> bitmap_size);
                //update bitmap
                new_node->bitmap[bitmap_index] = 1;
            }

            temp_old_lookup_table = temp_old_lookup_table -> down_lookup_table;
        }
    }
}

PBTrie_node * insert_pbTrie_node(char* read_string, PBTrie_node * now_node){
    printf("in \"insert_pbTrie_node\", MSG : check string is null or not\n");

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
        printf("\nin \"insert_pbTrie_node\", MSG : input message is : %s\n", input_string);
        //check branch exist of not
        
        PBTrie_node * check_branch_exist_or_not_pbTrie_node = check_branch_exist(temp_node, input_string);

        if(check_branch_exist_or_not_pbTrie_node == NULL){
            printf("in \"insert_pbTrie_node\", MSG : branch not exist\n");
            //branch not exist
            //add branch number
            temp_node -> branch_couter += 1;
            printf("in \"insert_pbTrie_node\", MSG : add branch_counter: %d\n", temp_node -> branch_couter);
            
            //compare bitsize number
            int old_bitmap_size = temp_node -> bitmap_size;
            temp_node -> bitmap_size = get_pow_value_of_2(temp_node -> branch_couter);
            printf("in \"insert_pbTrie_node\", MSG : old bitmap_size: %d, new bitmap_size: %d\n", old_bitmap_size, temp_node -> bitmap_size);

            //save old lookup table size
            int old_lookup_table_size = accumulate_i_of_bitmap(temp_node -> bitmap, temp_node -> bitmap_size);
            printf("in \"insert_pbTrie_node\", MSG : oold_lookup_table_size: %d\n", old_lookup_table_size);

            //chech need to realloc bitmap and lookup_table or not
            if(old_bitmap_size != temp_node -> bitmap_size){
                //bitmap size different, need to remalloc bitmap and lookup_table
                printf("in \"insert_pbTrie_node\", MSG : bitmap size different, need to remalloc bitmap and lookup_table\n");

                //create new bitmap
                free(temp_node -> bitmap);
                temp_node -> bitmap = (char * )calloc(temp_node -> bitmap_size, sizeof(char));

                //update_new_bitamp
                update_old_bitmap_to_new_bitmap(temp_node, temp_node -> lookup_table, old_lookup_table_size);

            }
            else; //bitmap size same, no need to remalloc bitmap and lookup_table

            
            
            //get bitmap index
            int bitmap_index =  get_bitmap_index(input_string, strlen(input_string), temp_node -> bitmap_size);
            printf("in \"insert_pbTrie_node\", MSG : get bitmap index : %d\n", bitmap_index);
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


            printf("in \"insert_pbTrie_node\", MSG : start malloc new branch\n");
            //malloc new branch
            PBTrie_node * new_pbTrie_node = create_new_branch(input_string);

            //accumulate bitmap's "1" to get lookup table index
            int lookup_table_index = accumulate_i_of_bitmap(temp_node -> bitmap, bitmap_index);
            printf("in \"insert_pbTrie_node\", MSG : get lookup table index : %d\n", lookup_table_index);
            //check a lookup table index is null or not
            if (temp_node -> lookup_table[lookup_table_index] == NULL){
                //lookup index is null, need to malloc
                printf("in \"insert_pbTrie_node\", MSG : lookup index is null, need to malloc\n");
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
            printf("in \"insert_pbTrie_node\", MSG : return new pbTrie node\n");
            return new_pbTrie_node;

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
        printf("in \"insert_pbTrie_node\", ERROR : input string is null\n");
        return now_node;
    }
}

void change_rule_id(PBTrie_node * now_node, int rule_id){
    if(now_node != NULL){
        printf("in \"change_rule_id\", MSG : change rule id : %d\n", rule_id);
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
        // check return pbTrie node is null or not
        while(strtok_string != NULL){
            //printf("%s", strtok_string);
            PBTrie_node * temp_pbTrie_node = insert_pbTrie_node(strtok_string, next_pbTrie_node);
            next_pbTrie_node = temp_pbTrie_node;
            strtok_string = strtok(NULL, tok);

            if(strtok_string == NULL){
                //this is the end of this prefix, change the rule id
                ++rule_count;
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
                int bitmap_index = get_bitmap_index(now_node -> lookup_table[i] -> pbTrie_node -> node_name, strlen(now_node -> lookup_table[i] -> pbTrie_node -> node_name), now_node -> bitmap_size);
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

        int father_bitmap_index = get_bitmap_index(temp_node -> node_name, strlen(temp_node -> node_name), father_node -> bitmap_size);
        
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
                        Lookup_Table * prew_lookup_table = father_node -> lookup_table[father_lookup_table_index];
                        Lookup_Table * temp_lookup_table = prew_lookup_table -> down_lookup_table;

                        while(temp_lookup_table != NULL){
                            if(temp_lookup_table -> pbTrie_node !=NULL){
                                if(strcmp(temp_node -> node_name, temp_lookup_table -> pbTrie_node -> node_name) == 0){
                                    prew_lookup_table -> down_lookup_table = temp_lookup_table -> down_lookup_table;
                                    free(temp_lookup_table);
                                    father_node -> branch_couter -= 1;
                                    break;
                                }

                                prew_lookup_table = temp_lookup_table;
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


int main(int argc, char* argv[]){
    init_pbTrie_root();

    if (argc >= 2){
        //build
        read_file_and_build_pbTrie(argv[1]);
        DFS_print_pbTrie(pbTrie_root);
    }
    if(argc >= 4){
        //delete
        read_file_and_delete_pbTrie(argv[3]);
        DFS_print_pbTrie(pbTrie_root);
    }
    if(argc >= 2){
        //search
        read_file_and_search_pbTrie(argv[1]);
    }
    if(argc >= 3){
        //update
        //read_file_and_build_pbTrie(argv[1]);
    }
    
    

    //printf(" 5. BKDR-Hash Function Value: %u\n",BKDRHash("google",1));


    //printf("argc : %d\n", argc);

    return 0;
}