licenses(["notice"])

cc_library(
    name = "centroid",
    hdrs = ["centroid.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay:random",
        "//Centroid/utils:NSGDist",
        "//Centroid/utils:beamSearch",
        "//Centroid/utils:check_nn_recall",
        "//Centroid/utils:parse_results",
        "//Centroid/utils:stats",
        "//Centroid/utils:types",
        "//Centroid/utils:graph",
        "//Centroid/utils:aspen_graph",
        "//Centroid/utils:aspen_flat_graph",
        "//Centroid/common:index",
    ],
)

cc_binary(
    name = "centroid_main",
    srcs = ["centroid.cc"],
    deps = [
        ":centroid",
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "//Centroid/utils:parse_command_line",
        "//Centroid/utils:time_loop",
        "//Centroid/utils:NSGDist",
        "//Centroid/utils:euclidean_point",
        "//Centroid/utils:point_range",
        "//Centroid/utils:mips_point",
        "//Centroid/utils:graph",
        "//Centroid/utils:aspen_graph",
        "//Centroid/utils:aspen_flat_graph",
    ],
)

package(
    default_visibility = ["//visibility:public"],
)
