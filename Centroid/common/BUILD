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
        "//Centroid/utils:types",
        "//Centroid/utils:beamSearch",
    ],
)

cc_library(
    name = "union_find",
    hdrs = ["union_find.h"],
    deps = [
        "@parlaylib//parlay:primitives",
    ],
)

package(
    default_visibility = ["//visibility:public"],
)
