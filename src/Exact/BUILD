package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "centroid",
    hdrs = ["centroid.h"],
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
        ":centroid",
    ],
)