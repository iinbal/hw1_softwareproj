#include <stdio.h>   
#include <stdlib.h>  
#include <math.h>

#define MIN_K 1
#define MIN_ITER 2
#define MAX_ITER 1000
#define DEFAULT_ITER 400
#define INITIAL_CAPACITY 10
#define MAX_LINE_LENGTH 1024
#define EPSILON 0.001

int validate_input(int argc, char *argv[], int *k, int *iterations);
double **load_input(int *num_vectors_ptr, int *dimension_ptr);
void kmeans(double **vectors, int num_vectors, int dimension, int k, int iterations);
void free_vectors_array(double **vectors, int num_vectors);
void print_result(double **centroids, int k, int dimension);
int is_number(double val);
double euclidean_distance(double *point1, double *point2, int dimension);

