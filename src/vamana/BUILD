licenses(["notice"])

cc_library(
    name = "index",
    hdrs = ["index.h"],
    deps = [
        "@parlaylib//parlay:parallel",
        "@parlaylib//parlay:primitives",
        "@parlaylib//parlay:random",
        "//src/utils:NSGDist",
        "//src/utils:point_range",
        "//src/utils:graph",
        "//src/vamana/utils:beamSearch",
        "//src/vamana/utils:types",
    ],
)

package(
    default_visibility = ["//visibility:public"],
)
