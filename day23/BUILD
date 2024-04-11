
cc_library(
    name = "finder",
    hdrs = ["finder.h"],
    srcs = ["finder.cc"],
    deps = [
        "@abseil-cpp//absl/strings",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/container:inlined_vector",
        "@abseil-cpp//absl/log:check",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":finder",
        "@abseil-cpp//absl/strings",
    ],
)