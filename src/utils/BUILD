licenses(["notice"])

cc_library(
    name = "check_nn_recall",
    hdrs = ["check_nn_recall.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "//Centroid/vamana/utils:beamSearch",
        "//Centroid/vamana/utils:types",
        "//Centroid/vamana/utils:stats",
        ":csvfile",
        ":parse_results",
    ],
)

cc_library(
    name = "csvfile",
    hdrs = ["csvfile.h"],
)

cc_library(
    name = "euclidean_point",
    hdrs = ["euclidean_point.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:file_map",
        ":NSGDist",
    ],
)

# cc_library(
#     name = "get_time",
#     hdrs = ["get_time.h"],
# )

cc_library(
    name = "graph",
    hdrs = ["graph.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:file_map",
    ],
)

# cc_library(
#     name = "IO",
#     hdrs = ["IO.h"],
#     deps = [
#         "@parlaylib//parlay:io",
#         "@parlaylib//parlay:parallel",
#         "@parlaylib//parlay:primitives",
#         "@parlaylib//parlay/internal:get_time",
#     ],
# )

cc_library(
    name = "heap",
    hdrs = ["heap.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
    ],
)

cc_library(
    name = "mips_point",
    hdrs = ["mips_point.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:file_map",
    ],
)

cc_library(
    name = "nn_types",
    hdrs = ["nn_types.h"],
    deps = [
        "//src/vamana:index",
        "//src/vamana/utils:types",
        "//src/vamana/utils:stats",
        "//src/vamana/utils:beamSearch",
        ":graph",
        ":union_find",
    ],
)

cc_library(
    name = "NSGDist",
    hdrs = ["NSGDist.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
    ],
)

cc_library(
    name = "parse_command_line",
    hdrs = ["parse_command_line.h"],
)

cc_library(
    name = "parse_results",
    hdrs = ["parse_results.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
    ],
)

cc_library(
    name = "point_range",
    hdrs = ["point_range.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:file_map",
    ],
)

cc_library(
    name = "union",
    hdrs = ["union.h"],
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