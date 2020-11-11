#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h> //isdigit()

int* solve_sat(int** sat);
int evalute_solution(int** sat, int* solution);
int* random_solution(int var_num);
int** random_ksat(int var_num, int cls_num, int k);
int** read_sat_from_file(char* filename);

void erase_solution(int* solution);
void erase_sat(int** sat);

void print_array(int* array);
void print_formula(int** formula);

//###################################################//

int main(int argc, char** argv){
    srand(123);
    int** sat = read_sat_from_file("sat");
    int* sol = solve_sat(sat);
    //print_formula(sat);
    print_array(sol);
    printf("%d\n",evalute_solution(sat,sol));
    erase_sat(sat);
    erase_solution(sol);
}

//###################################################//

int* solve_sat(int** sat){
    int var_num = sat[0][0];
    int cls_num = sat[0][1];

    int* current_solution = random_solution(var_num);
    int current_score = evalute_solution(sat, current_solution);

    int MAX_RETRIES = 100;
    double MAX_TEMPERATURE = 0.3;
    double MIN_TEMPERATURE = 0.0005;
    double DECAY_RATE = 1.0 / var_num;
    double current_temperature = MAX_TEMPERATURE;

    int i;
    for(i = 0; i < MAX_RETRIES; ++i){
        int j = 0;
        current_temperature = MAX_TEMPERATURE;
        while(current_temperature >= MIN_TEMPERATURE){
            current_temperature = MAX_TEMPERATURE * pow(M_E, (-j)*DECAY_RATE);
            int k;
            for(k = 1; k <= var_num ; k++){
                current_solution[k] = -current_solution[k];
                int new_score = evalute_solution(sat, current_solution);
                if(new_score == cls_num){
                    printf("finished at %d\n",i); 
                    return current_solution;
                }
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
                        current_solution[k] = -current_solution[k];
                    }
                }
            }
            ++j;
        }
    }
    printf("finished at %d\n",i);
    return current_solution;
}

int evalute_solution(int** sat, int* solution){ 
    int fitness = 0;
    int var_num = sat[0][0]; 
    int cls_num = sat[0][1];
    int i,j;
    for(i = 1; i <= cls_num; i++){
        for(j = 1; j <= sat[i][0]; j++){
            int literal = sat[i][j];
            if(literal == solution[abs(literal)]){
                ++fitness;
                break;
            }
        }
    }
    return fitness;
}

int* random_solution(int var_num){
    int* solution = malloc((var_num + 1) * sizeof(int));
    solution[0] = var_num;
    int i;
    for (i = 1; i <= var_num; i++){
        solution[i] = (rand() % 2) ? i : -i; 
    }
    return solution;
}

int** random_ksat(int var_num, int cls_num, int k){
    if (k > var_num) return NULL;
    int** ksat = malloc((cls_num + 1) * sizeof(int*));
    ksat[0] = malloc(2 * sizeof(int));
    ksat[0][0] = var_num; 
    ksat[0][1] = cls_num;
    int i,j;
    for(i = 1; i <= cls_num; i++){
        ksat[i] = malloc((k + 1) * sizeof(int));
        ksat[i][0] = k;
        int* set = calloc((var_num + 1), sizeof(int));
        for(j = 1; j <= k; j++){
            int to_set = 0;
            do{
                to_set = rand() % (var_num + 1);
                if(set[to_set] == 0){
                    set[to_set] = 1;
                }
                else{
                    to_set = 0;
                }
            }while(to_set == 0);
            ksat[i][j] = (rand() % 2) ? to_set : -to_set;
        }
        free(set);
    }
    return ksat;
}

int** read_sat_from_file(char* filename){ 
    // ++++ READ FILE VARIABLES ++++ //
    FILE* sat_file;
    char* line = NULL;
    size_t len = 0;
    size_t read;
    int clauses_number = 0;
    int variables_number = 0;
    int** sat;

    // ++++ READING FILE AND CREATING DATA STRUCTURE ++++ //
    sat_file = fopen(filename,"r");
    if (sat_file == NULL)
        return NULL;
    
    while ((read = getline(&line, &len, sat_file)) != -1) { // ommit commentary
        if(line[0] == 'c')
            continue;
        else
            break;
    }

    char* temp = line;
    while(temp){ // get number of variables from sat header
        if(isdigit(*temp)){
            variables_number = strtol(temp, &temp, 10);
            break;
        }
        else
            temp++;
    }

    while(temp){ // get number of clauses from sat header
        if(isdigit(*temp)){
            clauses_number = strtol(temp, &temp, 10);
            break;
        }
        else temp++;
    }

    sat = (int**)malloc(sizeof(int*) * (clauses_number + 1) ); // allocate clause arrays + header array

    sat[0] = (int*)malloc(sizeof(int) * 2); // header allocation
    sat[0][1] = clauses_number; // defining header
    sat[0][0] = variables_number; 

    int i = 1;
    int j = 1;

    while ((read = getline(&line, &len, sat_file)) != -1) { // load clauses to clauses data structure
        int number_of_literals = 0; // number of literals in given clause
        temp = line;
        while(*temp != '\0'){ // count literals in given clause due to " " termination
            if(*temp == ' ') number_of_literals++;
            temp++;
        }
        sat[i] = (int*)malloc(sizeof(int) * (number_of_literals + 1) ); // allocate memory for i clause 
        sat[i][0] = number_of_literals; // save number of variables in each clause
        temp = line;
        j = 1;
        while (*temp){ // find literals in clause, save them to structure
            if (  isdigit(*temp) || ( *temp=='-' && isdigit(*(temp+1)) )  ){
                int literal = strtol(temp, &temp, 10);
                if(literal) sat[i][j++] = literal;
            }
                
            else 
                temp++;
        }
        i++;
    }

    fclose(sat_file); // Finishing file usage

    if (line)
        free(line);
    
    return sat;
}

void erase_solution(int* solution){
    free(solution);
}

void erase_sat(int** sat){
    int cls_num = sat[0][1];
    int i;
    for(i = 0; i <= cls_num; i++){
        free(sat[i]);
    }
    free(sat);
}

void print_array(int* array){
    printf("******************\n");
    if(array){
        printf("%d: ", array[0]);
        int i;
        for(i = 1; i <= array[0]; i++){
            printf("%d, ", array[i]);
        }
        printf("\n");
    }
    else{
        printf("NULL ARRAY\n");
    }
}

void print_formula(int** formula){
    int i,j;
    printf("##################\n");
    printf("v: %d, c: %d\n",formula[0][0],formula[0][1]);
    printf("##################\n");
    int length = formula[0][1];
    for(i = 1; i <= length; i++){
        if(formula[i]){
            printf("%3d(%3d): ", i, formula[i][0]);
            for(j = 1; j<= formula[i][0]; j++){
                printf("%3d, ", formula[i][j]);
            }
            printf("\n");
        }
        else{
            printf("%3d(SAT):\n", i);
        }
    }
}
