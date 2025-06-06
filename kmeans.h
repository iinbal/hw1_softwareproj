#define MIN_K 1
#define MIN_ITER 1
#define MAX_ITER 1000
#define DEFAULT_ITER 400
#define INITIAL_CAPACITY 10
#define MAX_LINE_LENGTH 1024
#define EPSILON 0.001

int validate_input(int argc, char *argv[], int *k, int *iterations);
int is_number(double val);
int count_commas(const char *s);
double **load_input(int *num_vectors_ptr, int *dimension_ptr);
void free_vectors_array(double **vectors, int num_vectors);
double euclidean_distance(double *point1, double *point2, int dimension);

void initialize_memory(int k, double ***centroids_ptr, double ***new_centroids_sum_ptr, int **cluster_counts_ptr, int **assignments_ptr, int num_vectors, int dimension);
void free_centroids_memory(double **centroids, double **new_centroids_sum, int *cluster_counts, int *assignments, int k);
void copy_initial_centroids(double **centroids, double **vectors, int k, int dimension);
void assign_clusters(double **vectors, double **centroids, int *assignments, int num_vectors, int k, int dimension);
void compute_new_centroids(double **vectors, double **new_centroids_sum, int *cluster_counts, int *assignments, int num_vectors, int k, int dimension);
int update_centroids(double **centroids, double **new_centroids_sum, int *cluster_counts, int k, int dimension);

void kmeans(double **vectors, int num_vectors, int dimension, int k, int iterations);
void print_result(double **centroids, int k, int dimension);
