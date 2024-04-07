licenses(["notice"])

cc_library(
    name = "aspen_flat_graph",
    hdrs = ["aspen_flat_graph.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:file_map",
        "//external:cpam",
        "//external:pam",
        "//Centroid/utils/aspen_flat:aspen_flat",
        ":types",
        ":NSGDist",
    ],
)

cc_library(
    name = "aspen_graph",
    hdrs = ["aspen_graph.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:file_map",
        "//external:cpam",
        "//external:pam",
        "//Centroid/utils/aspen:aspen",
        ":types",
        ":NSGDist",
    ],
)

cc_library(
    name = "beamSearch",
    hdrs = ["beamSearch.h"],
    deps = [
        "@parlaylib//parlay:io",
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay:random",
        ":types",
        ":graph",
        ":stats",
    ],
)

cc_library(
    name = "check_nn_recall",
    hdrs = ["check_nn_recall.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        ":beamSearch",
        ":csvfile",
        ":parse_results",
        ":types",
        ":stats",
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
        ":types",
        ":NSGDist",
        ":parse_command_line",
    ],
)

cc_library(
    name = "get_time",
    hdrs = ["get_time.h"],
)

cc_library(
    name = "graph",
    hdrs = ["graph.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:file_map",
        ":types",
        ":NSGDist",
        ":parse_command_line",
    ],
)

cc_library(
    name = "IO",
    hdrs = ["IO.h"],
    deps = [
        "@parlaylib//parlay:io",
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:get_time",
    ],
)

cc_library(
    name = "mips_point",
    hdrs = ["mips_point.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:file_map",
        ":types",
        ":NSGDist",
        ":parse_command_line",
    ],
)

cc_library(
    name = "mmap",
    hdrs = ["mmap.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay/internal:file_map",
        ":parse_command_line",
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
        ":types",
        ":parse_command_line",
    ],
)

cc_library(
    name = "stats",
    hdrs = ["stats.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
    ],
)

cc_library(
    name = "time_loop",
    hdrs = ["time_loop.h"],
    deps = [
        ":get_time",
    ]
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        ":mmap"
    ],
)

cc_library(
    name = "union",
    hdrs = ["union.h"],
)

package(
    default_visibility = ["//visibility:public"],
)