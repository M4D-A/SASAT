//
// Created by M4DA - Adam Jagielski, jagielski.adamm@gmail.com
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> //isdigit()
#include "io.h"

int **read_sat_from_file(char *filename) {
    // ++++ READ FILE VARIABLES ++++ //
    FILE *sat_file;
    char *line = NULL;
    size_t len = 0;
    size_t read;
    int clauses_number = 0;
    int variables_number = 0;
    int **sat;

    // ++++ READING FILE AND CREATING DATA STRUCTURE ++++ //
    sat_file = fopen(filename, "r");
    if (sat_file == NULL)
        return NULL;

    while ((read = getline(&line, &len, sat_file)) != -1) { // ommit commentary
        if (line[0] == 'c')
            continue;
        else
            break;
    }

    char *temp = line;
    while (temp) { // get number of variables from sat header
        if (isdigit(*temp)) {
            variables_number = (int) strtol(temp, &temp, 10);
            break;
        } else
            temp++;
    }

    while (temp) { // get number of clauses from sat header
        if (isdigit(*temp)) {
            clauses_number = (int) strtol(temp, &temp, 10);
            break;
        } else temp++;
    }

    sat = (int **) malloc(sizeof(int *) * (clauses_number + 1)); // allocate clause arrays + header array

    sat[0] = (int *) malloc(sizeof(int) * 2); // header allocation
    sat[0][0] = clauses_number; // defining header
    sat[0][1] = variables_number;

    int i = 1;
    int j = 1;

    while ((read = getline(&line, &len, sat_file)) != -1) { // load clauses to clauses data structure
        int number_of_literals = 0; // number of literals in given clause
        temp = line;
        while (*temp != '\0') { // count literals in given clause due to " " termination
            if (*temp == ' ') number_of_literals++;
            temp++;
        }
        sat[i] = (int *) malloc(sizeof(int) * (number_of_literals + 1)); // allocate memory for i clause
        sat[i][0] = number_of_literals; // save number of variables in each clause
        temp = line;
        j = 1;
        while (*temp) { // find literals in clause, save them to structure
            if (isdigit(*temp) || (*temp == '-' && isdigit(*(temp + 1)))) {
                int literal = (int) strtol(temp, &temp, 10);
                if (literal) sat[i][j++] = literal;
            } else
                temp++;
        }
        i++;
    }

    fclose(sat_file); // Finishing file usage

    if (line)
        free(line);

    return sat;
}

void print_array(int *array) {
    printf("******************\n");
    if (array) {
        printf("%d: ", array[0]);
        int i;
        for (i = 1; i <= array[0]; i++) {
            printf("%d, ", array[i]);
        }
        printf("\n");
    } else {
        printf("NULL ARRAY\n");
    }
}

void print_formula(int **formula) {
    int i, j;
    printf("##################\n");
    printf("\\/: %d, ->: %d\n", formula[0][0], formula[0][1]);
    printf("##################\n");
    int length = formula[0][0];
    for (i = 1; i <= length; i++) {
        if (formula[i]) {
            printf("%3d(%3d): ", i, formula[i][0]);
            for (j = 1; j <= formula[i][0]; j++) {
                printf("%3d, ", formula[i][j]);
            }
            printf("\n");
        } else {
            printf("%3d(SAT):\n", i);
        }
    }
}