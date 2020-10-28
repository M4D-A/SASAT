#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int evalute_solution(int** sat, int* solution);
int* random_solution(int var_num);
int** random_ksat(int var_num, int cls_num, int k);

void erase_solution(int* solution);
void erase_sat(int** sat);

void print_array(int* array);
void print_formula(int** formula);

//###################################################//

int main(int argc, char** argv){
    srand(0);
    int i;
    for(i = 0; i<128;i++){
        int** sat = random_ksat(16,16,3);
        int* solution = random_solution(16);
        //print_array(solution);
        //print_formula(sat);
        //printf("%d\n", evalute_solution(sat, solution));
        erase_sat(sat);
        erase_solution(solution);
    }
}

//###################################################//

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
