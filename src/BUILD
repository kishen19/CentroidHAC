package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "centroid",
    hdrs = ["centroid.h"],
    deps = [
        "//src/vamana/utils:beamSearch",
        "//src/vamana/utils:stats",
        "//src/vamana/utils:types",
        "//src/utils:graph",
        "//src/utils:union_find",
        "//src/utils:heap",
    ],
)

cc_library(
    name = "centroid_bucket",
    hdrs = ["centroid_bucket.h"],
    deps = [
        "//src/vamana/utils:beamSearch",
        "//src/vamana/utils:stats",
        "//src/vamana/utils:types",
        "//src/utils:graph",
        "//src/utils:union_find",
    ],
)

cc_library(
    name = "centroid_exact",
    hdrs = ["centroid_exact.h"],
    deps = [
        "//src/utils:union_find",
        "//src/utils:nn_types",
    ],
)

cc_binary(
    name = "centroid_main",
    srcs = ["centroid.cc"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "//src/utils:parse_command_line",
        "//src/utils:euclidean_point",
        "//src/utils:point_range",
        "//src/utils:graph",
        "//src/utils:nn_types",
        ":centroid",
    ],
)

cc_binary(
    name = "centroid_bucket_main",
    srcs = ["centroid_bucket.cc"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "//src/utils:parse_command_line",
        "//src/utils:euclidean_point",
        "//src/utils:point_range",
        "//src/utils:graph",
        "//src/utils:nn_types",
        ":centroid_bucket",
    ],
)

cc_binary(
    name = "centroid_errors",
    srcs = ["centroid_errors.cc"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "//src/utils:parse_command_line",
        "//src/utils:euclidean_point",
        "//src/utils:point_range",
        "//src/utils:graph",
        "//src/utils:nn_types",
        ":centroid",
    ],
)

cc_binary(
    name = "centroid_exact_main",
    srcs = ["centroid_exact.cc"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "//src/utils:parse_command_line",
        "//src/utils:euclidean_point",
        "//src/utils:point_range",
        ":centroid_exact",
    ],
)

