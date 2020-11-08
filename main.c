#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h> //isdigit()

//#######################// 

const uint64_t m1  = 0x5555555555555555; 
const uint64_t m2  = 0x3333333333333333; 
const uint64_t m4  = 0x0f0f0f0f0f0f0f0f; 
const uint64_t m8  = 0x00ff00ff00ff00ff; 
const uint64_t m16 = 0x0000ffff0000ffff; 
const uint64_t m32 = 0x00000000ffffffff; 

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

struct solution* solve_sat(struct sat* fm);

struct solution* random_solution(uint64_t size);
struct clause* random_clause(uint64_t size);
struct clause* empty_clause(uint64_t size);
struct sat* random_sat(uint64_t cls_num, uint64_t var_num);
struct sat* empty_sat(uint64_t cls_num, uint64_t var_num);
struct sat* read_sat_from_file(char* filename);

uint64_t evaluate_clause(struct clause* cls, struct solution* sol);
uint64_t evaluate_sat(struct sat* fm, struct solution* sol);

void erase_solution(struct solution* sol);
void erase_clause(struct clause* cls);
void erase_sat(struct sat* fm);

uint64_t rand64();
uint64_t hamming_weight(uint64_t x);



void print_solution(struct solution* sol);
void print_clause(struct clause* sol);
void print_sat(struct sat* fm);

void pretty_print_solution(struct solution* sol);
void pretty_print_clause(struct clause* sol);
void pretty_print_sat(struct sat* fm);

//#######################//

int main(int argc, char** argv){
    srand(123);
    struct sat* fm = read_sat_from_file("sat");
    pretty_print_sat(fm);
    struct solution* sol = solve_sat(fm);
    pretty_print_solution(sol);
    printf("%lu\n",evaluate_sat(fm,sol));
}

//#######################//

struct solution* solve_sat(struct sat* fm){
    uint64_t var_num = fm->var_num;
    uint64_t cls_num = fm->cls_num;
    struct solution* current_solution = random_solution(var_num);
    uint64_t current_score = evaluate_sat(fm, current_solution);
    
    uint64_t MAX_RETRIES = 1;
    double MAX_TEMPERATURE = 0.4;
    double MIN_TEMPERATURE = 0.1;
    double DECAY_RATE = 1.0 / var_num;
    double current_temperature = MAX_TEMPERATURE;

    uint64_t i;
    for(i = 0; i < MAX_RETRIES; i++){
        uint64_t j = 0;
        while(current_temperature >= MIN_TEMPERATURE){
            current_temperature = MAX_TEMPERATURE * pow(M_E, (-j)*DECAY_RATE);
            uint64_t k;
            for(k = 0; k < var_num ; k++){
                uint64_t d = k / 64;
                uint64_t r = k % 64;
                current_solution->value[d] ^= (1 << r); 
                int new_score = evaluate_sat(fm, current_solution);
                if(new_score == cls_num) return current_solution;
                int delta = new_score - current_score;
                if(delta >= 0){
                    current_score = new_score;
                }
                else{
                    double threshold = (-delta) / current_temperature;
                    threshold = 1 + pow(M_E, threshold);
                    threshold = 1 / threshold;
                    double random_check = random() / RAND_MAX;
                    if(random_check < threshold){
                        current_score = new_score;
                    }
                    else{
                        current_solution->value[d] ^= (1 << r); 
                    }
                }
            }
            ++j;
        }
    }

    return current_solution;
}

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

uint64_t evaluate_clause(struct clause* cls, struct solution* sol){
    uint64_t length = cls->length;
    if(length != sol->length) return 0;

    uint64_t sat_sum = 0;
    uint64_t i;

    for(i = 0; i < length; i++){
        uint64_t word_relevance = cls->presence[0] & sol->presence[0];
        uint64_t word_cohesion = ~(cls->value[0] ^ sol->value[0]);
        uint64_t word_satisfaction = word_relevance & word_cohesion;
        sat_sum += hamming_weight(word_satisfaction);
    }

    return sat_sum;
}

uint64_t evaluate_sat(struct sat* fm, struct solution* sol){
    uint64_t cls_num = fm->cls_num;
    if(fm->var_num != sol->size) return 0; //Inconsistency

    uint64_t sat_sum = 0;
    uint64_t i;

    for(i = 0; i < cls_num; ++i){
        uint64_t cls_eval = evaluate_clause(fm->clauses[i], sol);
        if(cls_eval) ++sat_sum;
    }
    
    return sat_sum;
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

uint64_t rand64(){
    uint64_t retval = rand();
    retval <<= 32;
    retval |= rand();
    return retval;
}

uint64_t hamming_weight(uint64_t x){
    x = (x & m1 ) + ((x >>  1) & m1 ); //put count of each  2 bits into those  2 bits 
    x = (x & m2 ) + ((x >>  2) & m2 ); //put count of each  4 bits into those  4 bits 
    x = (x & m4 ) + ((x >>  4) & m4 ); //put count of each  8 bits into those  8 bits 
    x = (x & m8 ) + ((x >>  8) & m8 ); //put count of each 16 bits into those 16 bits 
    x = (x & m16) + ((x >> 16) & m16); //put count of each 32 bits into those 32 bits 
    x = (x & m32) + ((x >> 32) & m32); //put count of each 64 bits into those 64 bits 
    return x;
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

void pretty_print_solution(struct solution* sol){
    uint64_t length = sol->size;
    int64_t i;
    uint64_t positive;
    uint64_t negative;

    for(i = 0; i < length; i++){
        uint64_t r = i%64;
        uint64_t d = i/64;
        if(!r){
            positive =   sol->value[d]  & sol->presence[d];
            negative = ~(sol->value[d]) & sol->presence[d];
        }
        if(positive >> r & 1) printf("%ld ",(i+1));
        if(negative >> r & 1) printf("%ld ",-(i+1)); 
    }
    printf("0 \n");
}

void pretty_print_clause(struct clause* cls){
    pretty_print_solution((struct solution*) cls);
}

void pretty_print_sat(struct sat* fm){
    printf("p cnf %lu %lu\n",fm->var_num, fm->cls_num);
    uint64_t i;
    for (i = 0; i < fm->cls_num; i++){
        pretty_print_clause(fm->clauses[i]);
    }
}
