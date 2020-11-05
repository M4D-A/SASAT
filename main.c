#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h> //isdigit()

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
struct clause* empty_clause(uint64_t size);
struct sat* random_sat(uint64_t cls_num, uint64_t var_num);
struct sat* empty_sat(uint64_t cls_num, uint64_t var_num);
struct sat* read_sat_from_file(char* filename);

void erase_solution(struct solution* sol);
void erase_clause(struct clause* cls);
void erase_sat(struct sat* fm);

void print_solution(struct solution* sol);
void print_clause(struct clause* sol);
void print_sat(struct sat* fm);

uint64_t rand64();

//#######################//

int main(int argc, char** argv){
    srand(0);
    struct sat* fm = random_sat(2,70);
    erase_sat(fm);
    fm = read_sat_from_file("sat");
    erase_sat(fm);
    fm = empty_sat(12311,345);
    erase_sat(fm);
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
        retval->presence[retval->length - 1] = 0;
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

struct clause* empty_clause(uint64_t size){
    struct clause* retval = malloc(sizeof(struct clause));

    retval->size = size;
    retval->length = (size / 64);
    retval->residue = size % 64; 
    if(retval->residue) ++retval->length;
    retval->value = calloc(retval->length, sizeof(uint64_t));
    retval->presence = calloc(retval->length, sizeof(uint64_t));

    return retval;
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

struct sat* empty_sat(uint64_t cls_num, uint64_t var_num){
    struct sat* retval = malloc(sizeof(struct sat));

    retval->cls_num = cls_num;
    retval->var_num = var_num;
    retval->clauses = malloc(cls_num * sizeof(struct clause*));
    int i;
    for(i = 0; i < cls_num; i++){
        retval->clauses[i] = empty_clause(var_num);
    }

    return retval;
}

struct sat* read_sat_from_file(char* filename){ 
    // ++++ READ FILE VARIABLES ++++ //
    FILE* sat_file;
    char* line = NULL;
    size_t len = 0;
    size_t read;
    int cls_num = 0;
    int var_num = 0;
    struct sat* fm;

    // ++++ READING FILE AND CREATING DATA STRUCTURE ++++ //
    sat_file = fopen(filename,"r");
    if (sat_file == NULL) return NULL;
    
    while ((read = getline(&line, &len, sat_file)) != -1) { // ommit commentary
        if(line[0] == 'c')
            continue;
        else
            break;
    }

    char* temp = line;
    while(temp){ // get number of variables from sat header
        if(isdigit(*temp)){
            var_num = strtol(temp, &temp, 10);
            break;
        }
        else
            temp++;
    }

    while(temp){ // get number of clauses from sat header
        if(isdigit(*temp)){
            cls_num = strtol(temp, &temp, 10);
            break;
        }
        else temp++;
    }

    fm = empty_sat(cls_num, var_num);

    int i = 0;
    while ((read = getline(&line, &len, sat_file)) != -1) { // load clauses to clauses data structure
        temp = line;
        struct clause* current_cls = fm->clauses[i];
        while (*temp){ // find literals in clause, save them to structure
            if (  isdigit(*temp) || ( *temp=='-' && isdigit(*(temp+1)) )  ){
                int literal = strtol(temp, &temp, 10);
                if(literal){
                    uint64_t index = abs(literal) - 1;
                    uint64_t word = index / 64;
                    uint64_t position = index % 64;
                    uint64_t mask = 1;
                    mask <<= position;
                    current_cls->presence[word] |= mask;
                    if(literal > 0){
                        current_cls->value[word] |= mask;
                    }
                }
            }
            else temp++;
        }
        i++;
    }

    fclose(sat_file); // Finishing file usage
    if (line) free(line);

    return fm;
}

void erase_solution(struct solution* sol){
    free(sol->value);
    free(sol->presence);
    free(sol);
}

void erase_clause(struct clause* cls){
    free(cls->value);
    free(cls->presence);
    free(cls);
}

void erase_sat(struct sat* fm){
    int i;
    for(i = 0; i < fm->cls_num; i++){
        erase_clause(fm->clauses[i]);
    }
    
    free(fm->clauses);
    free(fm);
}

void print_solution(struct solution* sol){
    printf("#s:%8lu |l:%8lu |r:%8lu #\n",sol->size,sol->length,sol->residue);

    printf("| ");
    uint64_t i;
    for(i=sol->length - 1; i > 0; --i){
        printf("%016lx ", sol->value[i]);
    }
    printf("%016lx |\n",sol->value[0]);

    printf("| ");
    for(i=sol->length - 1; i > 0; --i){
        printf("%016lx ", sol->presence[i]);
    }
    printf("%016lx |\n",sol->presence[0]);
}

void print_clause(struct clause* cls){
    print_solution((struct solution*) cls);
}

void print_sat(struct sat* fm){
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@c:%8lu |v:%8lu @\n",fm->cls_num,fm->var_num);
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@\n");

    uint64_t i;
    for(i=0; i < fm->cls_num; i++){
        print_clause(fm->clauses[i]);
    }
}

uint64_t rand64(){
    uint64_t retval = rand();
    retval <<= 32;
    retval |= rand();
    return retval;
}