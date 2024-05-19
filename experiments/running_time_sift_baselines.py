import sys, time
import numpy as np
from tqdm import tqdm as tqdm
import multiprocessing as mp
from call_function_with_timeout import SetTimeoutDecorator

from baselines.exact_baselines import sklearn_HAC, fastcluster_HAC
from data_utils.get_datasets import get_iris_dataset, get_digits_dataset, get_wine_dataset, get_breast_cancer_dataset, get_faces_dataset

# BASE_PATH = "/ssd2/kishen/centroidHAC/sift/slices/"
BASE_PATH = "/home/kishen/CentroidHAC/data1/sift/slices/"

library_map = {
    "sklearn": sklearn_HAC,
    "fastcluster": fastcluster_HAC
}
sklearn_linkage = ["ward", "complete", "average", "single"]
fastcluster_linkage = ["single", "complete", "average", "weighted", "centroid", "median", "ward"]

@SetTimeoutDecorator(timeout=3*60*60) # 3 hours
def func(data):
    start = time.time()
    fastcluster_HAC(data, "centroid")
    tt = time.time()-start
    return tt

sizes=[32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1000000]
for size in sizes:
    dataFile = BASE_PATH + f"sift_{size}.fbin"
    
    # Read from binary file
    with open(dataFile, 'rb') as file:
        n = np.fromfile(file, dtype=np.int32, count=1)[0]
        dim = np.fromfile(file, dtype=np.int32, count=1)[0]
        data = np.fromfile(file, dtype=np.float32, count=n*dim)
    data = data.reshape((n, dim))
    n,dim = int(n),int(dim)
    print(f"FastCluster Centroid (sift {size})")
    tt = 0
    rounds = 5
    for i in range(rounds):
        is_done, is_timeout, erro_message, results = func(data.copy())
        if is_done:
            tt += results
        else:
            tt = 3*60*60*5 # 3 hours
            break
    print(f"Time taken for FastCluster (sift {size}): {tt/rounds}",flush=True)
    