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
algs = ["centroid_%.2f"%i for i in eps2] +["centroid_%.1f"%i for i in eps1]+["centroid_exact"]+[f"fastcluster_{linkage}" for linkage in fastcluster_linkage] + [f"sklearn_{linkage}" for linkage in sklearn_linkage]

output = {dataset:{} for dataset in basic_datasets}

for alg in algs:
    stats = json.load(open(BASE_PATH + f"basic/stats_{alg}.json"))
    for dataset in basic_datasets:
        for stat in stats[dataset]["best"]:
            if stat in output[dataset]:
                output[dataset][stat].append((alg,stats[dataset]["best"][stat]))
            else:
                output[dataset][stat] = [(alg,stats[dataset]["best"][stat])]

for dataset in basic_datasets:
    json.dump(output[dataset], open(BASE_PATH+f"basic/{dataset}/{dataset}_stats.json","w"), indent=2)

for dataset in basic_datasets:
    print(dataset)
    for stat in output[dataset]:
        print("\t"+stat)
        for alg, value in output[dataset][stat]:
            print(f"{value}")

# BASE_PATH = "/ssd2/kishen/centroidHAC/mnist/"
# algs = ["centroid_exact"]+["centroid_%.1f"%i for i in eps]
# output = {}
# for alg in algs:
#     stats = json.load(open(BASE_PATH + f"/stats_{alg}.json"))
#     for stat in stats["best"]:
#         if stat in output:
#             output[stat].append((alg,stats["best"][stat]))
#         else:
#             output[stat] = [(alg,stats["best"][stat])]

# print("MNIST")
# for stat in output:
#     print("\t"+stat)
#     for alg, value in output[stat]:
#         print(f"Alg {alg}: {value}")
