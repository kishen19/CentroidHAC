licenses(["notice"])

cc_library(
    name = "nn_types",
    hdrs = ["nn_types.h"],
    deps = [
        "//Centroid/vamana:index",
        "//Centroid/vamana/utils:beamSearch",
        "//Centroid/vamana/utils:types",
        "//Centroid/vamana/utils:stats",
        "//Centroid/utils:graph",
        "//Centroid/utils:union_find",
    ],
)

cc_library(
    name = "centroid",
    hdrs = ["centroid.h"],
    deps = [
        "//Centroid/vamana/utils:beamSearch",
        "//Centroid/vamana/utils:stats",
        "//Centroid/vamana/utils:types",
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
        ":nn_types",
    ],
)

cc_binary(
    name = "centroid_exact",
    srcs = ["centroid_exact.cc"],
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
        ":nn_types",
    ],
)

package(
    default_visibility = ["//visibility:public"],
)
