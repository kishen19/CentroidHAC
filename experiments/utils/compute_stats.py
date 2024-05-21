from sklearn import metrics
from tqdm import tqdm as tqdm
import higra as hg
import numpy as np
import time, sys

from utils.cut_dendrogram import make_cuts, make_all_cuts, read_dendrogram


def compute_ari_score(labels_true, labels_pred):
    """
    Compute the Adjusted Rand Index (ARI) score.
    
    Parameters:
    - labels_true: true labels of the data points
    - labels_pred: predicted labels of the data points
    
    Returns:
    - ari_score: the ARI score
    """
    ari_score = metrics.adjusted_rand_score(labels_true, labels_pred)
    return ari_score

def compute_ami_score(labels_true, labels_pred):
    """
    Compute the Adjusted Mutual Information (AMI) score.
    
    Parameters:
    - labels_true: true labels of the data points
    - labels_pred: predicted labels of the data points
    
    Returns:
    - ami_score: the AMI score
    """
    ami_score = metrics.adjusted_mutual_info_score(labels_true, labels_pred)
    return ami_score

def compute_completeness_score(labels_true, labels_pred):
    """
    Compute the Completeness score.
    
    Parameters:
    - labels_true: true labels of the data points
    - labels_pred: predicted labels of the data points
    
    Returns:
    - completeness_score: the Completeness score
    """
    completeness_score = metrics.completeness_score(labels_true, labels_pred)
    return completeness_score

def compute_homogeneity_score(labels_true, labels_pred):
    """
    Compute the Homogeneity score.
    
    Parameters:
    - labels_true: true labels of the data points
    - labels_pred: predicted labels of the data points
    
    Returns:
    - homogeneity_score: the Homogeneity score
    """
    homogeneity_score = metrics.homogeneity_score(labels_true, labels_pred)
    return homogeneity_score

def compute_v_measure_score(labels_true, labels_pred):
    """
    Compute the V-measure score.
    
    Parameters:
    - labels_true: true labels of the data points
    - labels_pred: predicted labels of the data points
    
    Returns:
    - v_measure_score: the V-measure score
    """
    v_measure_score = metrics.v_measure_score(labels_true, labels_pred)
    return v_measure_score

def compute_fowlkes_mallows_score(labels_true, labels_pred):
    """
    Compute the Fowlkes-Mallows score.
    
    Parameters:
    - labels_true: true labels of the data points
    - labels_pred: predicted labels of the data points
    
    Returns:
    - fowlkes_mallows_score: the Fowlkes-Mallows score
    """
    fowlkes_mallows_score = metrics.fowlkes_mallows_score(labels_true, labels_pred)
    return fowlkes_mallows_score

def dendrogram_purity(dend,labels_true):
    tree = hg.Tree(np.asarray(dend))
    return hg.dendrogram_purity(tree, labels_true)

def dasgupta_cost(dend,data):
    tree = hg.Tree(np.asarray(dend))
    graph,edge_weights = hg.make_graph_from_points(data,graph_type="complete")
    return hg.dasgupta_cost(tree,edge_weights,graph,mode="dissimilarity")

map_stats = {
    "ari_score": compute_ari_score,
    "ami_score": compute_ami_score,
    "completeness_score": compute_completeness_score,
    "homogeneity_score": compute_homogeneity_score,
    "v_measure_score": compute_v_measure_score,
    "fowlkes_mallows_score": compute_fowlkes_mallows_score,
}

def compute_stats_log_cuts(data, labels_true, dendrogram_file):
    total_stats = {
        "best": {stat:0 for stat in map_stats}
    }
    for cut, labels_pred in tqdm(make_cuts(dendrogram_file)):
        stats = {}
        for stat in map_stats:
            # if stat in ["silhouette_score", "calinski_harabasz_score", "davies_bouldin_score"]:
            #     stats[stat] = map_stats[stat](data, labels_pred)
            # else:
            stats[stat] = map_stats[stat](labels_true, labels_pred)
        total_stats[cut] = stats
        for stat in map_stats:
            total_stats["best"][stat] = max(total_stats["best"][stat], stats[stat])
    st = time.time()
    dp = dendrogram_purity(read_dendrogram(dendrogram_file)[1], labels_true)
    ed1 = time.time()
    print("DP time: ", ed1-st)
    dg = dasgupta_cost(read_dendrogram(dendrogram_file)[1], data)
    ed2 = time.time()
    print("DG time: ", ed2-ed1)
    total_stats["best"]["dendrogram_purity"] = dp
    total_stats["best"]["dasgupta_cost"] = dg
    return total_stats

def compute_stats_all_cuts(data, labels_true, dendrogram_file):
    total_stats = {
        "best": {stat:0 for stat in map_stats}
    }
    for cut, labels_pred in make_all_cuts(dendrogram_file):
        stats = {}
        for stat in map_stats:
            # if stat in ["silhouette_score", "calinski_harabasz_score", "davies_bouldin_score"]:
            #     stats[stat] = map_stats[stat](data, labels_pred)
            # else:
            stats[stat] = map_stats[stat](labels_true, labels_pred)
        total_stats[cut] = stats
        for stat in map_stats:
            total_stats["best"][stat] = max(total_stats["best"][stat], stats[stat])
    dp = dendrogram_purity(read_dendrogram(dendrogram_file)[1], labels_true)
    dg = dasgupta_cost(read_dendrogram(dendrogram_file)[1], data)
    total_stats["best"]["dendrogram_purity"] = dp
    total_stats["best"]["dasgupta_cost"] = dg
    return total_stats

def inversions_eps(dendrogram_file, eps = 0):
    n, dend, merge_costs = read_dendrogram(dendrogram_file)
    inv = 0
    one_plus_eps = 1 + eps
    for i in tqdm(range(n)):
        j = dend[i]
        while dend[j] != j:
            if merge_costs[dend[i]-n] < merge_costs[dend[j]-n]/one_plus_eps:
                inv += 1
            j = dend[j]
    return inv

def read_dendrogram(input_file):
    with open(input_file, "r") as f:
        data = f.readlines()
        n = int(data[0])
        parent = [int(data[i]) for i in range(1, 2*n)]
        merge_cost = [float(data[i]) for i in range(2*n, 3*n-1)]
    return n, parent, merge_cost

if __name__ == "__main__":
    n,dend,merge_costs = read_dendrogram("/ssd2/kishen/centroidHAC/basic/digits/dend/digits_dend_sklearn_average.txt")
    with open("/ssd2/kishen/centroidHAC/basic/digits/digits.txt", "r") as f:
        data = np.array([[float(i) for i in line.strip().split()] for line in f.readlines()][1:])
    print(dasgupta_cost(dend,data))