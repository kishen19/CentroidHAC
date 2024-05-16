import sys, json

from tqdm import tqdm as tqdm
from utils.compute_stats import inversions_eps
from data_utils.get_datasets import *

BASE_PATH = "/ssd2/kishen/centroidHAC/"
# BASE_PATH = "/home/kishen/CentroidHAC/data1/"

basic_datasets = {
    "iris": get_iris_dataset,
    "digits": get_digits_dataset,
    "wine": get_wine_dataset,
    "breast_cancer": get_breast_cancer_dataset,
    "faces": get_faces_dataset,
}

def compute_inversions_basic():
    method = sys.argv[2]
    param = sys.argv[3]
    for dataset in basic_datasets:
        if method == "centroid":
            dendrogram_file = BASE_PATH + f"basic/{dataset}/{dataset}_dend_{param}.txt"
        else:
            dendrogram_file = BASE_PATH + f"basic/{dataset}/{dataset}_dend_{method}_{param}.txt"
        for eps in [0,0.01,0.1,0.5,1]:
            inv = inversions_eps(dendrogram_file, eps)
            print(f"\nDataset: {dataset}, Method: {method}, Param: {param}, Eps: {eps}, Inversions: {inv}\n")

def compute_inversions_general():
    dataset = sys.argv[1]
    method = sys.argv[2]
    param = sys.argv[3]
    if method == "centroid":
        dendrogram_file = BASE_PATH + f"{dataset}/{dataset}_dend_{param}.txt"
    else:
        dendrogram_file = BASE_PATH + f"{dataset}/{dataset}_dend_{method}_{param}.txt"
    for eps in tqdm([1]):
        inv = inversions_eps(dendrogram_file, eps)
        print(f"\nDataset: {dataset}, Method: {method}, Param: {param}, Eps: {eps}, Inversions: {inv}\n")

if __name__ == "__main__":
    if sys.argv[1] == "basic":
        compute_inversions_basic()
    else:
        compute_inversions_general()