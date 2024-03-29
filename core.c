//
// Created by M4DA - Adam Jagielski, jagielski.adamm@gmail.com
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mem.h"
#include "core.h"

int *solve_sat(int **sat, int MAX_RETRIES, double MAX_TEMPERATURE, double MIN_TEMPERATURE) {
    int cls_num = sat[0][0];
    int var_num = sat[0][1];

    double DECAY_RATE = 1.0 / var_num;

    int **tsat = transpose_sat(sat);

    int *current_solution = base_solution(var_num);
    int *current_model = sat_model(sat, current_solution);
    int current_score = current_model[current_model[0]];

    int *new_solution = malloc((var_num + 1) * sizeof(int));
    int *new_model = malloc((cls_num + 2) * sizeof(int));
    int new_score;

    int *max_solution = malloc((var_num + 1) * sizeof(int));
    int *max_model = malloc((cls_num + 2) * sizeof(int));
    int max_score = current_score;

    int i, j, k;
    for (i = 0; i < MAX_RETRIES; ++i) {
        j = 0;
        double current_temperature = MAX_TEMPERATURE;
        while (current_temperature >= MIN_TEMPERATURE) {
            current_temperature = MAX_TEMPERATURE * pow(2.71828, (-j) * DECAY_RATE);
            for (k = 1; k <= var_num; k++) {
                copy_array(current_solution, new_solution);
                copy_array(current_model, new_model);
                new_score = alter_and_evaluate(tsat, new_solution, new_model, k);
                if (new_score == cls_num) {
                    erase_sat(tsat);
                    erase_array(current_model);
                    erase_array(current_solution);
                    erase_array(max_model);
                    erase_array(max_solution);
                    erase_array(new_model);
                    printf("finished at %d\n", i);
                    return new_solution;
                }

                if (new_score >= max_score) {
                    max_score = new_score;
                    copy_array(new_solution, max_solution);
                    copy_array(new_model, max_model);
                }

                int delta = new_score - current_score;
                if (delta >= 0) {
                    current_score = new_score;
                    copy_array(new_solution, current_solution);
                    copy_array(new_model, current_model);
                } else {
                    double threshold = (-delta) / current_temperature;
                    threshold = 1 + pow(2.71828, threshold);
                    threshold = 1 / threshold;
                    double random_check = random() / RAND_MAX;
                    if (random_check < threshold) {
                        current_score = new_score;
                        copy_array(new_solution, current_solution);
                        copy_array(new_model, current_model);
                    }
                }
            }
            ++j;
        }
    }

    erase_sat(tsat);
    erase_array(current_model);
    erase_array(current_solution);
    erase_array(max_model);
    erase_array(new_solution);
    erase_array(new_model);
    printf("finished at %d\n", i);
    return max_solution;
}

int evaluate_solution(int **sat, const int *solution) {
    int fitness = 0;
    int cls_num = sat[0][0];
    int var_num = sat[0][1];
    int i, j;
    for (i = 1; i <= cls_num; i++) {
        for (j = 1; j <= sat[i][0]; j++) {
            int literal = sat[i][j];
            if (literal == solution[abs(literal)]) {
                ++fitness;
                break;
            }
        }
    }
    return fitness;
}

int alter_and_evaluate(int **t_sat, int *solution, int *model, int flick) {
    int fitness = model[model[0]];
    int length;
    int i;

    if (flick == 0) {
        return fitness;
    }

    int old_val = solution[flick];
    solution[flick] = -solution[flick];
    length = t_sat[flick][0];
    for (i = 1; i <= length; i++) {
        int occurance = t_sat[flick][i];
        int pos = abs(occurance);
        if ((old_val > 0) == (occurance > 0)) {
            model[pos]--;
            if (model[pos] == 0) fitness--;
        } else {
            model[pos]++;
            if (model[pos] == 1) fitness++;
        }
    }

    model[model[0]] = fitness;
    return fitness;
}

int *sat_model(int **sat, const int *solution) {
    int cls_num = sat[0][0];
    int var_num = sat[0][1];
    int fitness = 0;
    int *model = calloc(cls_num + 2, sizeof(int));
    model[0] = cls_num + 1;
    int i, j;

    for (i = 1; i <= cls_num; i++) {
        for (j = 1; j <= sat[i][0]; j++) {
            int literal = sat[i][j];
            if (literal == solution[abs(literal)]) {
                ++model[i];
            }
        }
        if (model[i] > 0) fitness++;
    }
    model[cls_num + 1] = fitness;

    return model;
}

int *base_solution(int var_num) {
    int *solution = malloc((var_num + 1) * sizeof(int));
    solution[0] = var_num;
    int i;
    for (i = 1; i <= var_num; i++) {
        solution[i] = i;
    }
    return solution;
}

int **transpose_sat(int **sat) {
    int i, j;
    int cls_num = sat[0][0];
    int var_num = sat[0][1];

    int **t_sat = malloc((var_num + 1) * sizeof(int *));
    t_sat[0] = malloc(2 * sizeof(int));
    t_sat[0][0] = var_num;
    t_sat[0][1] = cls_num;

    int *occurances = calloc(var_num + 1, sizeof(int));
    occurances[0] = var_num;

    for (i = 1; i <= cls_num; ++i) {
        int length = sat[i][0];
        for (j = 1; j <= length; ++j) {
            ++occurances[abs(sat[i][j])];
        }
    }

    for (i = 1; i <= var_num; ++i) {
        t_sat[i] = malloc((occurances[i] + 1) * sizeof(int));
        t_sat[i][0] = occurances[i];
    }

    for (i = 1; i <= cls_num; ++i) {
        int length = sat[i][0];
        for (j = 1; j <= length; ++j) {
            int literal = sat[i][j];
            int variable = abs(literal);
            if (literal < 0) t_sat[variable][occurances[variable]--] = -i;
            if (literal > 0) t_sat[variable][occurances[variable]--] = i;
        }
    }

    free(occurances);
    return t_sat;
}

