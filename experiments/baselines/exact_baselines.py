import fastcluster
from sklearn.cluster import AgglomerativeClustering as AC
import numpy as np
import time

def write_dend(dendFile, parent, merge_cost):
    n = len(merge_cost)+1
    with open(dendFile, "w") as f:
        f.write(f"{n}\n")
        for i in range(2*n-1):
            f.write(f"{parent[i]}\n")
        for i in range(n-1):
            f.write(f"{merge_cost[i]}\n")

# Linkage: {single, complete, average, weighted, centroid, median, ward}
def fastcluster_HAC(data, linkage, dendFile=""):
    start=time.time()
    n = data.shape[0]
    dend = fastcluster.linkage(data, metric="euclidean", method=linkage)
    parent = [i for i in range(2*n-1)]
    merge_cost = [0 for i in range(n-1)]
    for i in range(n-1):
        parent[int(dend[i][0])] = i+n
        parent[int(dend[i][1])] = i+n
        merge_cost[i] = dend[i][2]
    if (dendFile):
        write_dend(dendFile, parent, merge_cost)

# Linkage: {ward, complete, average, single}
def sklearn_HAC(data, linkage, dendFile):
    n = data.shape[0]
    clusterer = AC(compute_full_tree=True, linkage=linkage, compute_distances=True)
    clusterer.fit(data)
    parent = [i for i in range(2*n-1)]
    children = clusterer.children_
    for i in range(n-1):
        parent[children[i][0]] = n+i
        parent[children[i][1]] = n+i
    merge_cost = clusterer.distances_
    if (dendFile):
        write_dend(dendFile, parent, merge_cost)

if __name__ == "__main__":
    datafile = input().strip()
    with open(datafile, "r") as f:
        data = np.array([[float(i) for i in line.strip().split()] for line in f.readlines()][1:])
    sklearn_HAC(data, "average", datafile[:-4]+"_dend_sklearn_average.txt")