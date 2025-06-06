#include <stdio.h>   
#include <stdlib.h>  
#include <math.h>

#define MIN_K 1
#define MIN_ITER 1
#define MAX_ITER 1000
#define DEFAULT_ITER 400
#define INITIAL_CAPACITY 10 
#define MAX_LINE_LENGTH 1024 
#define MAX_COORD_LENGTH 100 
#define EPSILON 0.001

int validate_input(int argc, char *argv[], int *k, int *iterations);
double **load_input(int *num_vectors_ptr, int *dimension_ptr);
void kmeans(double **vectors, int num_vectors, int dimension, int k, int iterations);
void free_vectors_array(double **vectors, int num_vectors);
void print_result(double **centroids, int k, int dimension);
int is_number(double val);
double euclidean_distance(double *point1, double *point2, int dimension);

int main(int argc, char **argv) {
    int k, iterations;
    int num_vectors = 0;
    int dimension = 0;
    double **vectors;

    if (!validate_input(argc, argv, &k, &iterations)) {
        return 1;
    }

    vectors = load_input(&num_vectors, &dimension);
    if (!vectors) {
        return 1; 
    }

    if (k >= num_vectors) {
        printf("Incorrect number of clusters!\n");
        free_vectors_array(vectors, num_vectors);
        return 1;
    }

    kmeans(vectors, num_vectors, dimension, k, iterations);
    free_vectors_array(vectors, num_vectors);
    return 0;
}

int validate_input(int argc, char *argv[], int *k, int *iterations) {
    char *endptr;
    double k_double;
    double iter_double;

    if (argc < 2 || argc > 3) {
        printf("An Error Has Occurred\n");
        return 0;
    }

    k_double = strtod(argv[1], &endptr);
    if (*endptr != '\0') {
        printf("Incorrect number of clusters!\n");  
        return 0;
    }

    if (!is_number(k_double)) {
    printf("Incorrect number of clusters!\n");
    return 0;
}
    if (k_double != floor(k_double)) {
        printf("Incorrect number of clusters!\n");  
        return 0;
    }

    if (k_double < MIN_K) { 
        printf("Incorrect number of clusters!\n");  
        return 0;
    }
    *k = (int)k_double;

    if (argc == 3) {
        iter_double = strtod(argv[2], &endptr);
        if (*endptr != '\0') {
            printf("Incorrect maximum iteration!\n");  
            return 0;
        }

        if (iter_double != floor(iter_double)) {
            printf("Incorrect maximum iteration!\n");  
            return 0;
        }

        if (iter_double <= MIN_ITER || iter_double >= MAX_ITER) {
            printf("Incorrect maximum iteration!\n");  
            return 0;
        }
        *iterations = (int)iter_double;
    } else {
        *iterations = DEFAULT_ITER;
    }

    return 1;
}

int is_number(double val) {
    if (val != val || val == HUGE_VAL || val == -HUGE_VAL) {
        return 0;
    }

    return 1;
}

int count_commas(const char *s) {
    int count = 0;
    int i = 0;

    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] == ',') count++;
    }
    return count;
}

double **load_input(int *num_vectors_ptr, int *dimension_ptr) {
    char line[MAX_LINE_LENGTH];
    double **vectors = NULL;
    int vector_count = 0;
    int dim = 0;
    int capacity = INITIAL_CAPACITY;
    int len = 0;
    double val = 0.0;
    int chars_read = 0;
    int i = 0;
    double **new_vectors;
    double *vec;
    const char *p;

    vectors = malloc(capacity * sizeof(double*));
    if (!vectors) {
        printf("An Error Has Occurred\n");
        return NULL;
    }

    while (fgets(line, MAX_LINE_LENGTH, stdin)) {
        len = 0;
        i = 0;

        while (line[len] != '\0') {
            if (line[len] == '\n') {
                line[len] = '\0';
                break;
            }
            len++;
        }

        if (line[0] == '\0') continue; 

        if (vector_count == 0) {
            dim = count_commas(line) + 1;
            if (dim <= 0) {
                printf("An Error Has Occurred\n");
                free(vectors);
                return NULL;
            }
            *dimension_ptr = dim;
        }

        if (vector_count == capacity) {
            capacity *= 2;
            new_vectors = realloc(vectors, capacity * sizeof(double*));
            if (!new_vectors) {
                printf("An Error Has Occurred\n");
                free_vectors_array(vectors, vector_count);
                return NULL;
            }
            vectors = new_vectors;
        }

        vec = malloc(dim * sizeof(double));
        if (!vec) {
            printf("An Error Has Occurred\n");
            free_vectors_array(vectors, vector_count);
            return NULL;
        }

        p = line;
        while (i < dim) {
            chars_read = 0;
            if (sscanf(p, " %lf%n", &val, &chars_read) != 1 || val != val || val == HUGE_VAL || val == -HUGE_VAL) {
                printf("An Error Has Occurred\n");
                free(vec);
                free_vectors_array(vectors, vector_count);
                return NULL;
            }
            vec[i++] = val;

            p += chars_read;
            if (i < dim) {
                if (*p != ',') {
                    printf("An Error Has Occurred\n");
                    free(vec);
                    free_vectors_array(vectors, vector_count);
                    return NULL;
                }
                p += 1;
            }
        }

        while (*p == ' ') p += 1;
        if (*p != '\0') {
            printf("An Error Has Occurred\n");
            free(vec);
            free_vectors_array(vectors, vector_count);
            return NULL;
        }

        vectors[vector_count++] = vec;
    }

    if (vector_count == 0) {
        printf("An Error Has Occurred\n");
        free(vectors);
        return NULL;
    }

    *num_vectors_ptr = vector_count;
    return vectors;
}

double euclidean_distance(double *point1, double *point2, int dimension) {
    double sum = 0.0;
    double diff;
    int i;
    
    for (i = 0; i < dimension; i++) {
        diff = point1[i] - point2[i];
        sum += diff * diff;
    }
    
    return sqrt(sum);
}

void kmeans(double **vectors, int num_vectors, int dimension, int k, int iterations) {
    double **centroids;
    double **new_centroids_sum;
    int *cluster_counts;
    int *assignments;
    int i = 0;
    int j = 0;
    int iter = 0;
    int c = 0;
    int d = 0;
    int v = 0;
    int cluster;
    double min_distance_sq; 
    int best_cluster = 0;
    double current_distance_sq = 0.0;
    double diff = 0.0;
    int converged = 0;
    double centroid_distance = 0.0;

    centroids = malloc(k * sizeof(double*));
    new_centroids_sum = malloc(k * sizeof(double*));
    cluster_counts = calloc(k, sizeof(int));
    assignments = calloc(num_vectors, sizeof(int)); 

    if (!centroids || !new_centroids_sum || !cluster_counts || !assignments) {
        printf("An Error Has Occurred\n");
        if (centroids) free(centroids);
        if (new_centroids_sum) free(new_centroids_sum);
        free(cluster_counts);
        free(assignments);
        return;
    }

    for (i = 0; i < k; i++) {
        centroids[i] = NULL;
        new_centroids_sum[i] = NULL;
    }

    for (i = 0; i < k; i++) {
        centroids[i] = malloc(dimension * sizeof(double));
        new_centroids_sum[i] = malloc(dimension * sizeof(double));
        if (!centroids[i] || !new_centroids_sum[i]) {
            printf("An Error Has Occurred\n");
            for (j = 0; j < k; j++) {
                if (centroids[j]) free(centroids[j]);
                if (new_centroids_sum[j]) free(new_centroids_sum[j]);
            }
            free(centroids);
            free(new_centroids_sum);
            free(cluster_counts);
            free(assignments);
            return;
        }
    }

    for (i = 0; i < k; i++) {
        for (j = 0; j < dimension; j++) {
            centroids[i][j] = vectors[i][j];
        }
    }

    for (iter = 0; iter < iterations; iter++) {
        for (c = 0; c < k; c++) {
            for (d = 0; d < dimension; d++) {
                new_centroids_sum[c][d] = 0.0;
            }
            cluster_counts[c] = 0;
        }

        for (v = 0; v < num_vectors; v++) {
            best_cluster = 0;
            min_distance_sq = 1e308; 
            for (c = 0; c < k; c++) {
                current_distance_sq = 0.0;
                for (d = 0; d < dimension; d++) {
                    diff = vectors[v][d] - centroids[c][d];
                    current_distance_sq += diff * diff;
                }

                if (current_distance_sq < min_distance_sq) {
                    min_distance_sq = current_distance_sq;
                    best_cluster = c;
                }
            }
            assignments[v] = best_cluster;
        }

        for (v = 0; v < num_vectors; v++) {
            cluster = assignments[v];
            for (d = 0; d < dimension; d++) {
                new_centroids_sum[cluster][d] += vectors[v][d];
            }
            cluster_counts[cluster]++;
        }

        converged = 1;
        for (c = 0; c < k; c++) {
            if (cluster_counts[c] > 0) {
                for (d = 0; d < dimension; d++) {
                    new_centroids_sum[c][d] /= cluster_counts[c];
                }
                
                centroid_distance = euclidean_distance(centroids[c], new_centroids_sum[c], dimension);
                if (centroid_distance > EPSILON) {
                    converged = 0;
                }
                
                for (d = 0; d < dimension; d++) {
                    centroids[c][d] = new_centroids_sum[c][d];
                }
            } else {
                printf("An Error Has Occurred\n");
            }
        }

        if (converged && iter > 0) { 
            break;
        }
    }

    print_result(centroids, k, dimension);
    for (i = 0; i < k; i++) {
        free(centroids[i]);
        free(new_centroids_sum[i]);
    }
    free(centroids);
    free(new_centroids_sum);
    free(cluster_counts);
    free(assignments);
}

void free_vectors_array(double **vectors, int num_vectors) {
    int i = 0;
    if (vectors) {
        for (i = 0; i < num_vectors; i++) {
            free(vectors[i]);
        }
        free(vectors);
    }
}

void print_result(double **centroids, int k, int dimension) {
    int i = 0;
    int j = 0;

    for (i = 0; i < k; i++) {
        for (j = 0; j < dimension; j++) {
            printf("%.4f", centroids[i][j]);
            if (j < dimension - 1) printf(",");
        }
        printf("\n");
    }
}