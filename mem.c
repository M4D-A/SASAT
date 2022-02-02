//
// Created by M4DA - Adam Jagielski, jagielski.adamm@gmail.com
//
#include <stdlib.h>
#include <string.h>
#include "mem.h"

void copy_array(int *source, int *destination) {
    memcpy(destination, source, (source[0] + 1) * sizeof(int));
}

void erase_array(int *solution) {
    free(solution);
}

void erase_sat(int **sat) {
    int cls_num = sat[0][0];
    int i;
    for (i = 0; i <= cls_num; i++) {
        free(sat[i]);
    }
    free(sat);
}
