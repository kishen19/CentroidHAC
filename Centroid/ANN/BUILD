licenses(["notice"])

cc_library(
    name = "vamana",
    hdrs = ["vamana.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay:random",
        "//Centroid/utils:NSGDist",
        "//Centroid/utils:point_range",
        "//Centroid/utils:graph",
        "//Centroid/ANN/utils:beamSearch",
        "//Centroid/ANN/utils:types",
    ],
)

package(
    default_visibility = ["//visibility:public"],
)
