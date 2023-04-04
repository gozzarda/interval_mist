load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "dp",
    srcs = ["dp.cpp"],
    hdrs = ["dp.hpp"],
    deps = ["//src:graph"],
)

cc_library(
    name = "greedy",
    srcs = ["greedy.cpp"],
    hdrs = ["greedy.hpp"],
    deps = [
        "dp",
        "//src:graph",
    ],
)

cc_library(
    name = "naive",
    srcs = ["naive.cpp"],
    hdrs = ["naive.hpp"],
    deps = ["//src:graph"],
)

cc_library(
    name = "path_cover",
    srcs = ["path_cover.cpp"],
    hdrs = ["path_cover.hpp"],
    deps = ["//src:graph"],
)