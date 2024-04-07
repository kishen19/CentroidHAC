licenses(["notice"])

cc_library(
    name = "aspen",
    srcs = ["aspen.h"],
    deps = [
        "//external:cpam",
        "//external:pam",
        ":utils",
        ":immutable_graph",
        ":versioned_graph",
    ]
)

cc_library(
    name = "immutable_graph",
    srcs = ["immutable_graph.h"],
    deps = [],
)

cc_library(
    name = "sequentialHT",
    srcs = ["sequentialHT.h"],
)

cc_library(
    name = "utils",
    srcs = ["utils.h"],
    deps = [
        "@parlaylib//parlay:primitives",
    ]
)

cc_library(
    name = "versioned_graph",
    srcs = ["versioned_graph.h"],
    deps = [
        ":sequentialHT",
    ]
)

package(
    default_visibility = ["//visibility:public"],
)