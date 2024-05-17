import sys, json

from utils.compute_stats import compute_stats_log_cuts, compute_stats_all_cuts, inversions_eps
from data_utils.get_datasets import *

# BASE_PATH = "/ssd2/kishen/centroidHAC/"
BASE_PATH = "/home/kishen/CentroidHAC/data1/"

basic_datasets = {
    "iris": get_iris_dataset,
    "digits": get_digits_dataset,
    "wine": get_wine_dataset,
    "breast_cancer": get_breast_cancer_dataset,
    "faces": get_faces_dataset,
}

sklearn_linkage = ["ward", "complete", "average", "single"]
fastcluster_linkage = ["single", "complete", "average", "weighted", "centroid", "median", "ward"]

# Computing Stats for basic datasets, given dendrogram paths
def run_basic_stats(map_dends, saveFile = ""):
    overall_stats = {}
    for dataset in basic_datasets:
        # Fetch the data and ground truth labeling
        data, gt = basic_datasets[dataset](False)
        # Compute the cuts
        stats = compute_stats_all_cuts(data, gt, map_dends[dataset])
        stats["Num Classes"] = len(set(gt))
        stats["Shape"] = data.shape
        overall_stats[dataset] = stats
    if saveFile:
        json.dump(overall_stats, open(saveFile, "w"), indent=2)
    else:
        print(overall_stats)
    
# Given Dendrogram and Ground Truth, compute stats
def run_general_stats(dendrogramFile, dataFile, gtFile, saveFile = ""):
    with open(dataFile, "r") as f:
        data = np.array([[float(i) for i in line.strip().split()] for line in f.readlines()][1:])
    with open(gtFile, "r") as f:
        gt = np.asarray([int(i) for i in f.readlines()])
    # Compute the cuts
    stats = compute_stats_log_cuts(data, gt, dendrogramFile)
    stats["Num Classes"] = len(set(gt))
    stats["Size"] = len(gt)
    if saveFile:
        json.dump(stats, open(saveFile, "w"), indent=2)
    else:
        print(stats)

# Computes the dendrogram paths for the basic datasets
# given method (and other relevent params)
def compute_dend_map(params):
    map_dends = {dataset: BASE_PATH+f"basic/{dataset}/dend/{dataset}_dend_{params[0]}_{params[1]}.txt" for dataset in basic_datasets}
    return map_dends
    
# Compute stats for all basic datasets
def basic_stats():
    params = [sys.argv[2], sys.argv[3]]
    save_file = BASE_PATH+"basic/stats/stats_%s.json"%('_'.join([str(i) for i in params]))
    run_basic_stats(compute_dend_map(params), save_file)

def general_stats():
    dataset = sys.argv[1]
    params = [sys.argv[2], sys.argv[3]]
    dendrogramFile = BASE_PATH+f"{dataset}/dend/{dataset}_dend_{params[0]}_{params[1]}.txt"
    gtFile = BASE_PATH+f"{dataset}/{dataset}.gt"
    dataFile = BASE_PATH+f"{dataset}/{dataset}.txt"
    saveFile = BASE_PATH+f"{dataset}/stats/stats_{'_'.join(params)}.json"
    run_general_stats(dendrogramFile, dataFile, gtFile, saveFile)

# def compute_inversions_basic():
#     method = sys.argv[2]
#     param = sys.argv[3]
#     for dataset in basic_datasets:
#         if method == "centroid":
#             dendrogram_file = BASE_PATH+f"/ssd2/kishen/centroidHAC/{dataset}/{dataset}_dend_{param}.txt"
#         else:
#             dendrogram_file = f"/ssd2/kishen/centroidHAC/{dataset}/{dataset}_dend_{method}_{param}.txt"
#         for eps in [0,0.01,0.1,0.5,1]:
#             inv = inversions_eps(dendrogram_file, eps)
#             print(f"Dataset: {dataset}, Method: {method}, Param: {param}, Eps: {eps}, Inversions: {inv}")

# def compute_inversions_general():
#     dataset = sys.argv[1]
#     method = sys.argv[2]
#     param = sys.argv[3]
#     dendrogram_file = BASE_PATH+f"{dataset}/dend/{dataset}_dend_{method}_{param}.txt"
#     for eps in [0,0.01,0.1,0.5,1]:
#         inv = inversions_eps(dendrogram_file, eps)
#         print(f"Dataset: {dataset}, Method: {method}, Param: {param}, Eps: {eps}, Inversions: {inv}")

if __name__ == "__main__":
    if sys.argv[1] == "basic":
        basic_stats()
    else:
        general_stats()