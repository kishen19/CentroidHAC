licenses(["notice"])

cc_library(
    name = "aspen_flat",
    hdrs = ["aspen_flat.h"],
    deps = [
        "//external:cpam",
        "//external:pam",
        "//Centroid/utils/aspen:versioned_graph",
        ":immutable_graph",
    ]
)

cc_library(
    name = "immutable_graph",
    hdrs = ["immutable_graph.h"],
    deps = [],
)

package(
    default_visibility = ["//visibility:public"],
)