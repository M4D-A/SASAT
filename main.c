#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h> //isdigit()

int* solve_sat(int** sat);

int evalute_solution(int** sat, int* solution);
int alter_and_evaluate(int** t_sat, int* solution, int* model, int flick);
int* sat_model(int** sat, int* solution);
int* base_solution(int var_num);
int** read_sat_from_file(char* filename);
int** transpose_sat(int** sat);

void erase_array(int* solution);
void erase_sat(int** sat);

void print_array(int* array);
void print_formula(int** formula);

//###################################################//

int main(int argc, char** argv){
    int** sat = read_sat_from_file("sat");
    int* sol = solve_sat(sat);
    print_array(sol);
    printf("%d \n", evalute_solution(sat,sol));
}

//###################################################//

int* solve_sat(int** sat){
    int cls_num = sat[0][0];
    int var_num = sat[0][1];

    int* current_solution = base_solution(var_num);
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
    int cls_num = sat[0][0]; 
    int var_num = sat[0][1];
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

int alter_and_evaluate(int** t_sat, int* solution, int* model, int flick){
    int fitness = model[model[0] + 1];
    int length;
    int i;

    if(flick == 0){
        return fitness;
    }
    
    int old_val = solution[flick];
    solution[flick] = -solution[flick];
    length = t_sat[flick][0];
    for(i = 1; i <= length; i++){
        int occurance = t_sat[flick][i];
        int pos = abs(occurance);
        if( (old_val > 0) == (occurance > 0) ) {
            model[pos]--;
            if(model[pos] == 0) fitness--;
        }
        else {
            model[pos]++;
            if(model[pos] == 1) fitness++;
        }
    }

    model[model[0]+1] = fitness;
    return fitness;
}

int* sat_model(int** sat, int* solution){
    int cls_num = sat[0][0]; 
    int var_num = sat[0][1];
    int fitness;
    int* model = calloc(cls_num + 2, sizeof(int));
    model[0] = cls_num;
    int i,j;

    for(i = 1; i <= cls_num; i++){
        for(j = 1; j <= sat[i][0]; j++){
            int literal = sat[i][j];
            if(literal == solution[abs(literal)]){
                ++model[i];
            }
        }
        if(model[i] > 0) fitness++;
    }
    model[cls_num + 1] = fitness;

    return model;
}

int* base_solution(int var_num){
    int* solution = malloc((var_num + 1) * sizeof(int));
    solution[0] = var_num;
    int i;
    for (i = 1; i <= var_num; i++){
        solution[i] = i; 
    }
    return solution;
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
    sat[0][0] = clauses_number; // defining header
    sat[0][1] = variables_number; 

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

int** transpose_sat(int** sat){
    int i,j;
    int cls_num = sat[0][0];
    int var_num = sat[0][1]; 

    int** t_sat = malloc( (var_num + 1) * sizeof(int*) );
    t_sat[0] = malloc( 2 * sizeof(int) );
    t_sat[0][0] = var_num;
    t_sat[0][1] = cls_num;

    int* occurances = calloc(var_num + 1, sizeof(int));
    occurances[0] = var_num;

    for(i = 1; i <= cls_num; ++i){
        int length = sat[i][0];
        for(j = 1; j <= length; ++j){
            ++occurances[abs(sat[i][j])];
        }
    }

    for(i = 1; i <= var_num; ++i){
        t_sat[i] = malloc( (occurances[i] + 1) * sizeof(int) );
        t_sat[i][0] = occurances[i];
    }

    for(i = 1; i <= cls_num; ++i){
        int length = sat[i][0];
        for(j = 1; j <= length; ++j){
            int literal = sat[i][j];
            int variable = abs(literal);
            if(literal < 0) t_sat[variable][occurances[variable]--] = -i;
            if(literal > 0) t_sat[variable][occurances[variable]--] = i;
        }
    }

    free(occurances);
    return t_sat;
}

void erase_array(int* solution){
    free(solution);
}

void erase_sat(int** sat){
    int cls_num = sat[0][0];
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
    printf("\\/: %d, ->: %d\n",formula[0][0],formula[0][1]);
    printf("##################\n");
    int length = formula[0][0];
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

int** random_ksat(int var_num, int cls_num, int k){
    if (k > var_num) return NULL;
    int** ksat = malloc((cls_num + 1) * sizeof(int*));
    ksat[0] = malloc(2 * sizeof(int));
    ksat[0][0] = cls_num; 
    ksat[0][1] = var_num;
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