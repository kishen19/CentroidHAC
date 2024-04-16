licenses(["notice"])

cc_library(
    name = "nn_types",
    hdrs = ["nn_types.h"],
    deps = [
        "//Centroid/ANN:vamana",
        "//Centroid/ANN/utils:beamSearch",
        "//Centroid/ANN/utils:types",
        "//Centroid/ANN/utils:stats",
        "//Centroid/utils:graph",
        "//Centroid/utils:union_find",
    ],
)

#include "Centroid/ANN/utils/beamSearch.h"
#include "Centroid/ANN/utils/types.h"
#include "Centroid/ANN/utils/stats.h"
#include "Centroid/utils/graph.h"
#include "Centroid/utils/union_find.h"

cc_library(
    name = "centroid",
    hdrs = ["centroid.h"],
    deps = [
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
        ":nn_types",
    ],
)

package(
    default_visibility = ["//visibility:public"],
)
