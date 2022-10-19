load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "graph",
    visibility = ["//src:__subpackages__"],
    srcs = ["graph.cpp"],
    hdrs = ["graph.hpp"],
    deps = ["interval"],
)

cc_library(
    name = "interval",
    srcs = ["interval.cpp"],
    hdrs = ["interval.hpp"],
)

cc_library(
    name = "solvers",
    hdrs = ["solvers.hpp"],
    deps = [
        "//src/solvers:dp",
        "//src/solvers:greedy",
        "//src/solvers:naive",
        "//src/solvers:path_cover",
    ],
)

cc_library(
    name = "tester",
    srcs = ["tester.cpp"],
    hdrs = ["tester.hpp"],
    deps = ["graph"],
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        ":graph",
        "//src/graph:tree_transform",
        ":interval",
        ":solvers",
        ":tester",
    ],
)