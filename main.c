#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

//#######################//

struct solution{
    uint64_t size;
    uint64_t length;
    uint64_t residue;
    uint64_t* value;
    uint64_t* presence;
};
struct clause{
    uint64_t size;
    uint64_t length;
    uint64_t residue;
    uint64_t* value;
    uint64_t* presence;
};
struct sat{
    uint64_t cls_num;
    uint64_t var_num;
    struct clause** clauses;
};

//#######################//

struct solution* random_solution(uint64_t size);
struct clause* random_clause(uint64_t size);
struct sat* random_sat(uint64_t cls_num, uint64_t var_num);

void print_solution(struct solution* sol);
void print_clause(struct clause* sol);
void print_sat(struct sat* formulae);

uint64_t rand64();

//#######################//

int main(int argc, char** argv){
    srand(0);
    struct sat* formulae = random_sat(8,81);
    print_sat(formulae);
}

//#######################//

struct solution* random_solution(uint64_t size){
    struct solution* retval = malloc(sizeof(struct solution));
    retval->size = size;
    retval->length = (size / 64);
    retval->residue = size % 64; 
    if(retval->residue) ++retval->length;
    retval->value = malloc(retval->length * sizeof(uint64_t));
    retval->presence = malloc(retval->length * sizeof(uint64_t));
    uint64_t i;
    for(i=0; i < retval->length; i++){
        retval->value[i] = rand64();
    }
    for(i=0; i < retval->length - 1; i++){
        retval->presence[i] = -1;
    }
    if(retval->residue){
        for(i=0; i < retval->residue; i++){
            retval->presence[retval->length - 1] <<= 1;
            retval->presence[retval->length - 1] |= 1;
        }
    }
    else{
        retval->presence[retval->length - 1] = -1;
    }
    return retval;
}

struct clause* random_clause(uint64_t size){
    struct clause* cls = (struct clause*)random_solution(size);
    int i;
    for(i = 0; i < cls->length; i++){
        cls->presence[i] &= rand64();
    }
    return cls;
}

struct sat* random_sat(uint64_t cls_num, uint64_t var_num){
    struct sat* retval = malloc(sizeof(struct sat));
    retval->cls_num = cls_num;
    retval->var_num = var_num;
    retval->clauses = malloc(cls_num * sizeof(struct clause*));
    int i;
    for(i = 0; i < cls_num; i++){
        retval->clauses[i] = random_clause(var_num);
    }
    return retval;
}

void print_solution(struct solution* sol){
    printf("###|%4lu|%4lu|%4lu|###\n | ",sol->size,sol->length,sol->residue);

    uint64_t i;
    for(i=sol->length - 1; i > 0; --i){
        printf("%016lx | ", sol->value[i]);
    }
    printf("%016lx | \n | ",sol->value[0]);

    for(i=sol->length - 1; i > 0; --i){
        printf("%016lx | ", sol->presence[i]);
    }
    printf("%016lx | \n",sol->presence[0]);
}

void print_clause(struct clause* cls){
    print_solution((struct solution*) cls);
}

void print_sat(struct sat* formulae){
    printf("-------------------------\n");
    printf("@@@|%8lu|%8lu|@@@\n",formulae->cls_num,formulae->var_num);
    printf("-------------------------\n");
    uint64_t i;
    for(i=0; i < formulae->cls_num; i++){
        print_clause(formulae->clauses[i]);
    }
}

uint64_t rand64(){
    uint64_t retval = rand();
    retval <<= 32;
    retval |= rand();
    return retval;
}