licenses(["notice"])

cc_library(
    name = "index",
    hdrs = ["index.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay:random",
        "//Centroid/utils:NSGDist",
        "//Centroid/utils:point_range",
        "//Centroid/utils:graph",
        "//Centroid/vamana/utils:beamSearch",
        "//Centroid/vamana/utils:types",
    ],
)

package(
    default_visibility = ["//visibility:public"],
)
