import sys, time
import numpy as np

from baselines.exact_baselines import sklearn_HAC, fastcluster_HAC
from data_utils.get_datasets import get_iris_dataset, get_digits_dataset, get_wine_dataset, get_breast_cancer_dataset

BASE_PATH = "/ssd2/kishen/centroidHAC/"

library_map = {
    "sklearn": sklearn_HAC,
    "fastcluster": fastcluster_HAC
}
sklearn_linkage = ["ward", "complete", "average", "single"]
fastcluster_linkage = ["single", "complete", "average", "weighted", "centroid", "median", "ward"]

basic_datasets = {
    "iris": get_iris_dataset,
    "digits": get_digits_dataset,
    "wine": get_wine_dataset,
    "breast_cancer": get_breast_cancer_dataset,
}

def compute_dend_basic():
    for dataset in basic_datasets:
        data, _ = basic_datasets[dataset](False)
        for linkage in fastcluster_linkage:
            start = time.time()
            fastcluster_HAC(data, linkage, f"/ssd2/kishen/centroidHAC/basic/{dataset}/{dataset}_dend_fastcluster_{linkage}.txt")
            print(f"Time taken for {dataset} (fastcluster {linkage}): {time.time()-start}")
        for linkage in sklearn_linkage:
            start = time.time()
            sklearn_HAC(data, linkage, f"/ssd2/kishen/centroidHAC/basic/{dataset}/{dataset}_dend_sklearn_{linkage}.txt")
            print(f"Time taken for {dataset} (sklearn {linkage}): {time.time()-start}")

def compute_dend_general():
    dataset = sys.argv[1]
    library = sys.argv[2]
    linkages = [sys.argv[3]]
    if linkages[0] == "all":
        linkages = fastcluster_linkage if library == "fastcluster" else sklearn_linkage
    dataFile = BASE_PATH + f"{dataset}/{dataset}.txt"
    with open(dataFile, "r") as f:
        data = f.readlines()
        data = np.asarray([data[i].split() for i in range(1,len(data))],dtype=float)
    for linkage in linkages:
        dendFile = BASE_PATH + f"{dataset}/{dataset}_dend_{library}_{linkage}.txt"
        start = time.time()
        print(f"Computing Dendrogram for {dataset} ({library} {linkage})")
        library_map[library](data.copy(), linkage, dendFile)
        print(f"Time taken for {dataset} ({library} {linkage}): {time.time()-start}")

if __name__ == "__main__":
    param = sys.argv[1]
    if param == "basic":
        compute_dend_basic()
    else:
        compute_dend_general()