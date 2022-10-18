load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "bfs",
    srcs = ["bfs.cpp"],
    hdrs = ["bfs.hpp"],
    deps = ["//src:graph"],
)

cc_test(
  name = "bfs_test",
  size = "small",
  srcs = ["bfs_test.cpp"],
  deps = [
    "@com_google_googletest//:gtest_main",
    "bfs",
  ],
)

cc_library(
    name = "hamiltonian",
    srcs = ["hamiltonian.cpp"],
    hdrs = ["hamiltonian.hpp"],
    deps = ["//src:graph"],
)

cc_test(
  name = "hamiltonian_test",
  size = "small",
  srcs = ["hamiltonian_test.cpp"],
  deps = [
    "@com_google_googletest//:gtest_main",
    "hamiltonian",
  ],
)