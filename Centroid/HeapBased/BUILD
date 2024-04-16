licenses(["notice"])

cc_library(
    name = "centroid",
    hdrs = ["centroid.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay:random",
        "//Centroid/ANN:vamana",
        "//Centroid/ANN/utils:beamSearch",
        "//Centroid/ANN/utils:stats",
        "//Centroid/ANN/utils:types",
        "//Centroid/utils:graph",
        "//Centroid/utils:union_find",
    ],
)

cc_binary(
    name = "centroid_main",
    srcs = ["centroid.cc"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "//Centroid/utils:parse_command_line",
        "//Centroid/utils:time_loop",
        "//Centroid/utils:euclidean_point",
        "//Centroid/utils:point_range",
        "//Centroid/utils:mips_point",
        "//Centroid/utils:graph",
        ":centroid",
    ],
)

package(
    default_visibility = ["//visibility:public"],
)
