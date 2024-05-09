import sys, json

from utils.compute_stats import compute_stats_log_cuts, compute_stats_all_cuts
from data_utils.get_datasets import *

BASE_PATH = "/ssd2/kishen/centroidHAC/"

basic_datasets = {
    "iris": get_iris_dataset,
    "digits": get_digits_dataset,
    "wine": get_wine_dataset,
    "breast_cancer": get_breast_cancer_dataset,
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
        stats = compute_stats_all_cuts(gt, map_dends[dataset])
        stats["Num Classes"] = len(set(gt))
        stats["Shape"] = data.shape
        overall_stats[dataset] = stats
    if saveFile:
        json.dump(overall_stats, open(saveFile, "w"), indent=2)
    else:
        print(overall_stats)
    
# Given Dendrogram and Ground Truth, compute stats
def run_general_stats(dendrogramFile, gtFile, saveFile = ""):
    with open(gtFile, "r") as f:
        gt = np.asarray([int(i) for i in f.readlines()])
    # Compute the cuts
    stats = compute_stats_log_cuts(gt, dendrogramFile)
    stats["Num Classes"] = len(set(gt))
    stats["Size"] = len(gt)
    if saveFile:
        json.dump(stats, open(saveFile, "w"), indent=2)
    else:
        print(stats)

# Computes the dendrogram paths for the basic datasets
# given method (and other relevent params)
def compute_dend_map(params):
    if params[0] == "centroid":
        map_dends = { dataset: BASE_PATH+f"basic/{dataset}/{dataset}_dend_{params[1]}.txt" for dataset in basic_datasets}
        return map_dends
    elif params[0] == "centroid_exact":
        map_dends = { dataset: BASE_PATH+f"basic/{dataset}/{dataset}_dend_exact.txt" for dataset in basic_datasets}
        return map_dends
    elif params[0] == "sklearn":
        map_dends = { dataset: BASE_PATH+f"basic/{dataset}/{dataset}_dend_sklearn_{params[1]}.txt" for dataset in basic_datasets}
        return map_dends
    elif params[0] == "fastcluster":
        map_dends = { dataset: BASE_PATH+f"basic/{dataset}/{dataset}_dend_fastcluster_{params[1]}.txt" for dataset in basic_datasets}
        return map_dends
    else:
        sys.exit(1)

# Compute stats for all basic datasets
def basic_stats():
    params = [sys.argv[2]]
    if params[0] in ["centroid", "sklearn", "fastcluster"]:
        params.append(sys.argv[3])
    save_file = BASE_PATH+"basic/stats_%s.json"%('_'.join([str(i) for i in params]))
    run_basic_stats(compute_dend_map(params), save_file)

def general_stats():
    dataset = sys.argv[1]
    params = [sys.argv[2]]
    if sys.argv[2] == "centroid":
        params.append(sys.argv[3])
        dendrogramFile = BASE_PATH+f"{dataset}/{dataset}_dend_{sys.argv[3]}.txt"
    elif sys.argv[2] == "centroid_exact":
        dendrogramFile = BASE_PATH+f"{dataset}/{dataset}_dend_exact.txt"
    elif sys.argv[2] == "sklearn":
        params.append(sys.argv[3])
        dendrogramFile = BASE_PATH+f"{dataset}/{dataset}_dend_sklearn_{sys.argv[3]}.txt"
    elif sys.argv[2] == "fastcluster":
        params.append(sys.argv[3])
        dendrogramFile = BASE_PATH+f"{dataset}/{dataset}_dend_fastcluster_{sys.argv[3]}.txt"
    gtFile = BASE_PATH+f"{dataset}/{dataset}.gt"
    saveFile = BASE_PATH+f"{dataset}/stats_{'_'.join(params)}.json"
    run_general_stats(dendrogramFile, gtFile, saveFile)

if __name__ == "__main__":
    if sys.argv[1] == "basic":
        basic_stats()
    else:
        general_stats()