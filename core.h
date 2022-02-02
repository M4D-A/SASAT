//
// Created by M4DA - Adam Jagielski, jagielski.adamm@gmail.com
//

#ifndef SASAT_CORE_H
#define SASAT_CORE_H
#endif //SASAT_CORE_H

int *solve_sat(int **sat, int MAX_RETRIES, double MAX_TEMPERATURE, double MIN_TEMPERATURE);

int evaluate_solution(int **sat, const int *solution);

int alter_and_evaluate(int **t_sat, int *solution, int *model, int flick);

int *sat_model(int **sat, const int *solution);

int *base_solution(int var_num);

int **transpose_sat(int **sat);