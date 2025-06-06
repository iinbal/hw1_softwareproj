import sys

# Constants
MIN_K = 1
MIN_ITER = 1
MAX_ITER = 1000
DEFAULT_ITER = 400

def is_number(s):
    if s is None or s.strip() == '':
        return False
    try:
        val = float(s)
    except ValueError:
        return False
    lower_s = s.strip().lower()
    return lower_s not in {'nan', 'inf', '+inf', '-inf'}

#Command line parsing

def parse_command_line_args():
    args = sys.argv[1:]

    if not args:
        return None, None, None

    k = parse_k_arg(args[0])
    if k is None:
        return None, None, None

    max_iter = parse_iter_arg(args[1]) if len(args) > 1 else DEFAULT_ITER
    if max_iter is None:
        return None, None, None

    file_arg = args[2] if len(args) > 2 else None
    return k, max_iter, file_arg

def parse_k_arg(arg):
    if not is_number(arg):
        print("Incorrect number of clusters!")
        return None
    k_float = float(arg)
    if k_float != int(k_float) or int(k_float) < MIN_K:
        print("Incorrect number of clusters!")
        return None
    return int(k_float)

def parse_iter_arg(arg):
    if not is_number(arg):
        print("Incorrect maximum iteration!")
        return None
    iter_float = float(arg)
    if iter_float != int(iter_float):
        print("Incorrect maximum iteration!")
        return None
    iter_int = int(iter_float)
    if iter_int < MIN_ITER or iter_int >= MAX_ITER:
        print("Incorrect maximum iteration!")
        return None
    return iter_int

#Vector reading and validation

def read_vectors(file_arg=None):
    try:
        lines = read_lines(file_arg)
        vectors = parse_vectors(lines)
        if not vectors:
            raise ValueError
        return vectors
    except Exception:
        print("An Error Has Occurred")
        return None

def read_lines(file_arg):
    if file_arg:
        with open(file_arg, 'r') as f:
            return f.readlines()
    return sys.stdin.readlines()

def parse_vectors(lines):
    vectors = []
    for line in lines:
        line = line.strip()
        if line:
            try:
                vector = list(map(float, line.split(',')))
                vectors.append(vector)
            except ValueError:
                raise
    return vectors

def validate_vector_dimensions(vectors):
    if not vectors:
        print("An Error Has Occurred")
        return False
    dim = len(vectors[0])
    for vector in vectors:
        if len(vector) != dim:
            print("An Error Has Occurred")
            return False
    return True

# K-Means Funcs

def initialize_centroids(vectors, k):
    if k >= len(vectors):
        print("Incorrect number of clusters!")
        return None, None
    return [vectors[i][:] for i in range(k)], k

def euclidean_distance(point, centroid):
    return sum((point[i] - centroid[i]) ** 2 for i in range(len(point))) ** 0.5

def assign_clusters(vectors, centroids):
    clusters = [[] for _ in range(len(centroids))]
    for vector in vectors:
        distances = [euclidean_distance(vector, c) for c in centroids]
        clusters[distances.index(min(distances))].append(vector)
    return clusters

def update_centroids(clusters, centroids, eps):
    new_centroids = []
    converged = True
    for i, cluster in enumerate(clusters):
        if cluster:
            new_centroid = compute_mean(cluster)
            if euclidean_distance(centroids[i], new_centroid) > eps:
                converged = False
            new_centroids.append(new_centroid)
        else:
            print("An Error Has Occurred")
            return None, False
    return new_centroids, converged

def compute_mean(cluster):
    dim = len(cluster[0])
    summed = [0] * dim
    for point in cluster:
        for i in range(dim):
            summed[i] += point[i]
    return [x / len(cluster) for x in summed]

def print_results(clusters, centroids, verbose=True):
    for centroid in centroids:
        print(','.join(f"{x:.4f}" for x in centroid))

def kmeans_clustering(k, max_iter, vectors, eps=0.001, verbose=True):
    if not validate_vector_dimensions(vectors):
        return None

    centroids, _ = initialize_centroids(vectors, k)
    if centroids is None:
        return None

    for iteration in range(max_iter):
        clusters = assign_clusters(vectors, centroids)
        centroids, converged = update_centroids(clusters, centroids, eps)
        if converged and iteration > 0:
            break

    print_results(clusters, centroids, verbose)
    return clusters

#  Main 
def main():
    k, max_iter, file_arg = parse_command_line_args()
    if k is None or max_iter is None:
        return 1

    vectors = read_vectors(file_arg)
    if vectors is None:
        return 1

    if kmeans_clustering(k, max_iter, vectors) is None:
        return 1

    return 0

if __name__ == "__main__":
    sys.exit(main())
