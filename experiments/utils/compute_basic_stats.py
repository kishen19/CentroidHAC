from sklearn import metrics

from utils.cut_dendrogram import make_cuts


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

def compute_silhouette_score(data, labels):
    """
    Compute the Silhouette score.
    
    Parameters:
    - data: data points
    - labels: cluster labels of the data points
    
    Returns:
    - silhouette_score: the Silhouette score
    """
    silhouette_score = metrics.silhouette_score(data, labels)
    return silhouette_score

def compute_calinski_harabasz_score(data, labels):
    """
    Compute the Calinski-Harabasz score.
    
    Parameters:
    - data: data points
    - labels: cluster labels of the data points
    
    Returns:
    - calinski_harabasz_score: the Calinski-Harabasz score
    """
    calinski_harabasz_score = metrics.calinski_harabasz_score(data, labels)
    return calinski_harabasz_score

def compute_davies_bouldin_score(data, labels):
    """
    Compute the Davies-Bouldin score.
    
    Parameters:
    - data: data points
    - labels: cluster labels of the data points
    
    Returns:
    - davies_bouldin_score: the Davies-Bouldin score
    """
    davies_bouldin_score = metrics.davies_bouldin_score(data, labels)
    return davies_bouldin_score

# TODO: Implement the following functions
def dendrogram_purity():
    pass

def dasgupta_cost():
    pass

map_stats = {
    "ari_score": compute_ari_score,
    "ami_score": compute_ami_score,
    "completeness_score": compute_completeness_score,
    "homogeneity_score": compute_homogeneity_score,
    "v_measure_score": compute_v_measure_score,
    "fowlkes_mallows_score": compute_fowlkes_mallows_score,
    # "silhouette_score": compute_silhouette_score,
    # "calinski_harabasz_score": compute_calinski_harabasz_score,
    # "davies_bouldin_score": compute_davies_bouldin_score,
}

def compute_stats(data, labels_true, dendrogram_file):
    total_stats = {
        "best": {stat:0 for stat in map_stats}
    }
    for cut, labels_pred in make_cuts(dendrogram_file):
        stats = {}
        for stat in map_stats:
            # if stat in ["silhouette_score", "calinski_harabasz_score", "davies_bouldin_score"]:
            #     stats[stat] = map_stats[stat](data, labels_pred)
            # else:
            stats[stat] = map_stats[stat](labels_true, labels_pred)
        total_stats[cut] = stats
        for stat in map_stats:
            total_stats["best"][stat] = max(total_stats["best"][stat], stats[stat])
    return total_stats