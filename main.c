//
// Created by M4DA - Adam Jagielski, jagielski.adamm@gmail.com
//

#include <stdio.h>
#include "mem.h"
#include "io.h"
#include "core.h"

int main(int argc, char **argv) {
    int **sat = read_sat_from_file(argv[1]);
    int *sol = solve_sat(sat, 200, 0.3, 0.001);
    print_array(sol);
    printf("%d \n", evaluate_solution(sat, sol));
    erase_sat(sat);
    erase_array(sol);
}