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
void cleanup_kmeans_memory(double **centroids, double **new_centroids_sum, 
                          int *cluster_counts, int *assignments, int k);

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

int is_valid_argc(int argc) {
    return argc >= 2 && argc <= 3;
}

void print_error_occurred() {
    printf("An Error Has Occurred\n");
}

void print_cluster_error() {
    printf("Incorrect number of clusters!\n");
}

void print_iteration_error() {
    printf("Incorrect maximum iteration!\n");
}

int is_string_fully_parsed(char *endptr) {
    return *endptr == '\0';
}

int is_integer(double value) {
    return value == floor(value);
}

int is_valid_k_range(double k_double) {
    return k_double >= MIN_K;
}

int is_valid_iter_range(double iter_double) {
    return iter_double >= MIN_ITER && iter_double <= MAX_ITER;
}

int parse_k_value(char *arg, int *k) {
    char *endptr;
    double k_double = strtod(arg, &endptr);
    
    if (!is_string_fully_parsed(endptr)) {
        print_cluster_error();
        return 0;
    }

    if (!is_number(k_double)) {
        print_cluster_error();
        return 0;
    }
    
    if (!is_integer(k_double)) {
        print_cluster_error();
        return 0;
    }

    if (!is_valid_k_range(k_double)) {
        print_cluster_error();
        return 0;
    }
    
    *k = (int)k_double;
    return 1;
}

int parse_iter_value(char *arg, int *iterations) {
    char *endptr;
    double iter_double = strtod(arg, &endptr);
    
    if (!is_string_fully_parsed(endptr)) {
        print_iteration_error();
        return 0;
    }

    if (!is_number(iter_double)) {
        print_iteration_error();
        return 0;
    }

    if (!is_integer(iter_double)) {
        print_iteration_error();
        return 0;
    }

    if (!is_valid_iter_range(iter_double)) {
        print_iteration_error();
        return 0;
    }
    
    *iterations = (int)iter_double;
    return 1;
}

int validate_input(int argc, char *argv[], int *k, int *iterations) {
    if (!is_valid_argc(argc)) {
        print_error_occurred();
        return 0;
    }

    if (!parse_k_value(argv[1], k)) {
        return 0;
    }

    if (argc == 3) {
        if (!parse_iter_value(argv[2], iterations)) {
            return 0;
        }
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

void remove_newline(char *line) {
    int len = 0;
    while (line[len] != '\0') {
        if (line[len] == '\n') {
            line[len] = '\0';
            break;
        }
        len++;
    }
}

int is_empty_line(char *line) {
    return line[0] == '\0';
}

int calculate_dimension(char *line) {
    int dim = count_commas(line) + 1;
    if (dim <= 0) {
        print_error_occurred();
        return -1;
    }
    return dim;
}

double **resize_vectors_array(double **vectors, int new_capacity) {
    double **new_vectors = realloc(vectors, new_capacity * sizeof(double*));
    if (!new_vectors) {
        print_error_occurred();
        return NULL;
    }
    return new_vectors;
}

double *allocate_vector(int dimension) {
    double *vec = malloc(dimension * sizeof(double));
    if (!vec) {
        print_error_occurred();
        return NULL;
    }
    return vec;
}

int is_valid_number_for_parsing(double val) {
    return !(val != val || val == HUGE_VAL || val == -HUGE_VAL);
}

int parse_coordinate(const char **p, double *val) {
    int chars_read = 0;
    if (sscanf(*p, " %lf%n", val, &chars_read) != 1 || !is_valid_number_for_parsing(*val)) {
        print_error_occurred();
        return 0;
    }
    *p += chars_read;
    return 1;
}

int expect_comma(const char **p) {
    if (**p != ',') {
        print_error_occurred();
        return 0;
    }
    *p += 1;
    return 1;
}

void skip_spaces(const char **p) {
    while (**p == ' ') *p += 1;
}

int is_end_of_line(const char *p) {
    return *p == '\0';
}

double *parse_vector_from_line(char *line, int dimension) {
    double *vec = allocate_vector(dimension);
    if (!vec) return NULL;

    const char *p = line;
    int i = 0;
    double val;

    while (i < dimension) {
        if (!parse_coordinate(&p, &val)) {
            free(vec);
            return NULL;
        }
        vec[i++] = val;

        if (i < dimension) {
            if (!expect_comma(&p)) {
                free(vec);
                return NULL;
            }
        }
    }

    skip_spaces(&p);
    if (!is_end_of_line(p)) {
        print_error_occurred();
        free(vec);
        return NULL;
    }

    return vec;
}

double **load_input(int *num_vectors_ptr, int *dimension_ptr) {
    char line[MAX_LINE_LENGTH];
    double **vectors = NULL;
    int vector_count = 0;
    int dim = 0;
    int capacity = INITIAL_CAPACITY;
    double **new_vectors;
    double *vec;

    vectors = malloc(capacity * sizeof(double*));
    if (!vectors) {
        print_error_occurred();
        return NULL;
    }

    while (fgets(line, MAX_LINE_LENGTH, stdin)) {
        remove_newline(line);
        if (is_empty_line(line)) continue;

        if (vector_count == 0) {
            dim = calculate_dimension(line);
            if (dim == -1) {
                free(vectors);
                return NULL;
            }
            *dimension_ptr = dim;
        }

        if (vector_count == capacity) {
            capacity *= 2;
            new_vectors = resize_vectors_array(vectors, capacity);
            if (!new_vectors) {
                free_vectors_array(vectors, vector_count);
                return NULL;
            }
            vectors = new_vectors;
        }

        vec = parse_vector_from_line(line, dim);
        if (!vec) {
            free_vectors_array(vectors, vector_count);
            return NULL;
        }

        vectors[vector_count++] = vec;
    }

    if (vector_count == 0) {
        print_error_occurred();
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

void free_centroids(double **centroids, int k) {
    int i;
    if (centroids) {
        for (i = 0; i < k; i++) {
            if (centroids[i]) free(centroids[i]);
        }
        free(centroids);
    }
}

void cleanup_kmeans_memory(double **centroids, double **new_centroids_sum, 
                          int *cluster_counts, int *assignments, int k) {
    free_centroids(centroids, k);
    free_centroids(new_centroids_sum, k);
    if (cluster_counts) free(cluster_counts);
    if (assignments) free(assignments);
}

int allocate_kmeans_arrays(double ***centroids, double ***new_centroids_sum, 
                          int **cluster_counts, int **assignments, int k, int num_vectors) {
    *centroids = malloc(k * sizeof(double*));
    *new_centroids_sum = malloc(k * sizeof(double*));
    *cluster_counts = calloc(k, sizeof(int));
    *assignments = calloc(num_vectors, sizeof(int));

    return *centroids && *new_centroids_sum && *cluster_counts && *assignments;
}

int allocate_centroid_vectors(double **centroids, double **new_centroids_sum, int k, int dimension) {
    int i;
    for (i = 0; i < k; i++) {
        centroids[i] = malloc(dimension * sizeof(double));
        new_centroids_sum[i] = malloc(dimension * sizeof(double));
        if (!centroids[i] || !new_centroids_sum[i]) {
            return 0;
        }
    }
    return 1;
}

void initialize_centroids(double **centroids, double **vectors, int k, int dimension) {
    int i, j;
    for (i = 0; i < k; i++) {
        for (j = 0; j < dimension; j++) {
            centroids[i][j] = vectors[i][j];
        }
    }
}

void reset_cluster_data(double **new_centroids_sum, int *cluster_counts, int k, int dimension) {
    int c, d;
    for (c = 0; c < k; c++) {
        for (d = 0; d < dimension; d++) {
            new_centroids_sum[c][d] = 0.0;
        }
        cluster_counts[c] = 0;
    }
}

int find_closest_cluster(double *vector, double **centroids, int k, int dimension) {
    int best_cluster = 0;
    double min_distance = euclidean_distance(vector, centroids[0], dimension);
    double current_distance;
    int c;
    
    for (c = 1; c < k; c++) {
        current_distance = euclidean_distance(vector, centroids[c], dimension);
        if (current_distance < min_distance) {
            min_distance = current_distance;
            best_cluster = c;
        }
    }
    return best_cluster;
}

void assign_vectors_to_clusters(double **vectors, double **centroids, int *assignments, 
                               int num_vectors, int k, int dimension) {
    int v;
    for (v = 0; v < num_vectors; v++) {
        assignments[v] = find_closest_cluster(vectors[v], centroids, k, dimension);
    }
}

void accumulate_cluster_sums(double **vectors, double **new_centroids_sum, int *cluster_counts, 
                           int *assignments, int num_vectors, int dimension) {
    int v, d, cluster;
    for (v = 0; v < num_vectors; v++) {
        cluster = assignments[v];
        for (d = 0; d < dimension; d++) {
            new_centroids_sum[cluster][d] += vectors[v][d];
        }
        cluster_counts[cluster]++;
    }
}

void calculate_new_centroid(double *new_centroid_sum, int cluster_count, int dimension) {
    int d;
    for (d = 0; d < dimension; d++) {
        new_centroid_sum[d] /= cluster_count;
    }
}

void copy_centroid(double *dest, double *src, int dimension) {
    int d;
    for (d = 0; d < dimension; d++) {
        dest[d] = src[d];
    }
}

int check_convergence_and_update(double **centroids, double **new_centroids_sum, 
                                int *cluster_counts, int k, int dimension) {
    int converged = 1;
    int c;
    double centroid_distance;
    
    for (c = 0; c < k; c++) {
        if (cluster_counts[c] > 0) {
            calculate_new_centroid(new_centroids_sum[c], cluster_counts[c], dimension);
            
            centroid_distance = euclidean_distance(centroids[c], new_centroids_sum[c], dimension);
            if (centroid_distance > EPSILON) {
                converged = 0;
            }
            
            copy_centroid(centroids[c], new_centroids_sum[c], dimension);
        } else {
            print_error_occurred();
            return -1;
        }
    }
    return converged;
}

int should_stop_iteration(int converged, int iter) {
    return converged && iter > 0;
}

void kmeans(double **vectors, int num_vectors, int dimension, int k, int iterations) {
    double **centroids;
    double **new_centroids_sum;
    int *cluster_counts;
    int *assignments;
    int iter;
    int converged;

    if (!allocate_kmeans_arrays(&centroids, &new_centroids_sum, &cluster_counts, &assignments, k, num_vectors)) {
        print_error_occurred();
        cleanup_kmeans_memory(centroids, new_centroids_sum, cluster_counts, assignments, k);
        return;
    }

    if (!allocate_centroid_vectors(centroids, new_centroids_sum, k, dimension)) {
        print_error_occurred();
        cleanup_kmeans_memory(centroids, new_centroids_sum, cluster_counts, assignments, k);
        return;
    }

    initialize_centroids(centroids, vectors, k, dimension);

    for (iter = 0; iter < iterations; iter++) {
        reset_cluster_data(new_centroids_sum, cluster_counts, k, dimension);
        assign_vectors_to_clusters(vectors, centroids, assignments, num_vectors, k, dimension);
        accumulate_cluster_sums(vectors, new_centroids_sum, cluster_counts, assignments, num_vectors, dimension);
        
        converged = check_convergence_and_update(centroids, new_centroids_sum, cluster_counts, k, dimension);
        if (converged == -1) {
            cleanup_kmeans_memory(centroids, new_centroids_sum, cluster_counts, assignments, k);
            return;
        }

        if (should_stop_iteration(converged, iter)) {
            break;
        }
    }

    print_result(centroids, k, dimension);
    cleanup_kmeans_memory(centroids, new_centroids_sum, cluster_counts, assignments, k);
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