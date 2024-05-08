import sys, json

from utils.compute_basic_stats import compute_stats
from data_utils.get_datasets import *

basic_datasets = {
    "iris": get_iris_dataset,
    "digits": get_digits_dataset,
    "wine": get_wine_dataset,
    "breast_cancer": get_breast_cancer_dataset,
    # "california_housing": get_california_housing_dataset,
    # "diabetes": get_diabetes_dataset,
    # "linnerud": get_linnerud_dataset
}

# Computing Stats, given dendrogram path
def run_stats(map_dends, saveFile = ""):
    overall_stats = {}
    for dataset in basic_datasets:
        # Fetch the data and ground truth labeling
        data, gt = basic_datasets[dataset](False)
        # Compute the cuts
        stats = compute_stats(data, gt, map_dends[dataset])
        stats["Num Classes"] = len(set(gt))
        stats["Shape"] = data.shape
        overall_stats[dataset] = stats
    if saveFile:
        json.dump(overall_stats, open(saveFile, "w"), indent=2)
    else:
        print(overall_stats)

def compute_dend_map(params):
    if params[0] == "centroid":
        map_dends = { dataset: "/ssd2/kishen/centroidHAC/basic/%s/%s_dend_%.1f.txt"%(dataset,dataset,params[1]) for dataset in basic_datasets}
        return map_dends
    elif params[0] == "centroid_exact":
        map_dends = { dataset: "/ssd2/kishen/centroidHAC/basic/%s/%s_dend_exact.txt"%(dataset,dataset) for dataset in basic_datasets}
        return map_dends
    else:
        return {}

if __name__ == "__main__":
    params = [sys.argv[1]]
    if params[0] == "centroid":
        params.append(float(sys.argv[2]))
    save_file = "/ssd2/kishen/centroidHAC/basic/stats_%s.json"%("_".join([str(i) for i in params]))
    run_stats(compute_dend_map(params), save_file)