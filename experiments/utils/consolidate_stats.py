import json

BASE_PATH = "/ssd2/kishen/centroidHAC/"
# BASE_PATH = "/home/kishen/CentroidHAC/data1/"

basic_datasets = [
    "iris",
    "wine",
    "breast_cancer",
    "digits",
    "faces",
]

eps2 = [0.01,0.05]
eps1 = [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9]
sklearn_linkage = ["ward", "complete", "average", "single"]
fastcluster_linkage = ["single", "complete", "average", "weighted", "centroid", "median", "ward"]

algs = ["centroid_%.2f"%i for i in eps2] +["centroid_%.1f"%i for i in eps1]
algs += ["centroid_bucket_%.2f"%i for i in eps2] +["centroid_bucket_%.1f"%i for i in eps1]
algs += ["centroid_exact"]+[f"fastcluster_{linkage}" for linkage in fastcluster_linkage] 
algs += [f"sklearn_{linkage}" for linkage in sklearn_linkage]

output = {dataset:{} for dataset in basic_datasets}

for alg in algs:
    stats = json.load(open(BASE_PATH + f"basic/stats/stats_{alg}.json"))
    for dataset in basic_datasets:
        for stat in stats[dataset]["best"]:
            if stat in output[dataset]:
                output[dataset][stat][alg] = stats[dataset]["best"][stat]
            else:
                output[dataset][stat] = {alg:stats[dataset]["best"][stat]}

for dataset in basic_datasets:
    json.dump(output[dataset], open(BASE_PATH+f"basic/{dataset}/stats/{dataset}_stats.json","w"), indent=2)

# algs1 = ["centroid_%.1f"%i for i in [0.1,0.2,0.4,0.8]]+[f"fastcluster_{lin}" for lin in ["centroid","single","complete","average","ward"]]
algs1 = ["centroid_%.1f"%i for i in [0.1,0.2,0.4,0.8]]+["centroid_exact", "fastcluster_centroid"]

for stat in output["iris"]:
    print(stat)
    for dataset in basic_datasets:
        print(dataset)
        for alg in algs1:
            print("%.3f"%output[dataset][stat][alg],end=" & ")
        print()

# BASE_PATH = "/ssd2/kishen/centroidHAC/mnist/"
# output = {}
# for alg in algs:
#     stats = json.load(open(BASE_PATH + f"stats/stats_{alg}.json"))
#     for stat in stats["best"]:
#         print(stat)
#         if stat in output:
#             output[stat][alg]=stats["best"][stat]
#         else:
#             output[stat] = {alg:stats["best"][stat]}

# # json.dump(output, open(BASE_PATH+f"stats/mnist_stats.json","w"), indent=2)
# print("MNIST")
# for stat in output:
#     print("#"+stat)
#     for alg in algs1:
#         print("%.3f"%output[stat][alg],end=" & ")
#     print()