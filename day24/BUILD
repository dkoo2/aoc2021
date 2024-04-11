
cc_library(
    name = "types",
    hdrs = ["types.h"],
    deps = [
        "@abseil-cpp//absl/strings",
    ],
)

cc_library(
    name = "thread_pool",
    hdrs = ["thread_pool.h"],
    deps = [
        "@abseil-cpp//absl/functional:any_invocable",
        "@abseil-cpp//absl/synchronization",
    ],
)

cc_library(
    name = "instruction",
    hdrs = ["instruction.h"],
    srcs = ["instruction.cc"],
    deps = [
        ":types",
        "@abseil-cpp//absl/strings",
        "@abseil-cpp//absl/log:check",
    ],
)

cc_library(
    name = "parser",
    hdrs = ["parser.h"],
    srcs = ["parser.cc"],
    deps = [
        ":instruction",
        ":thread_pool",
        "@abseil-cpp//absl/strings",
        "@abseil-cpp//absl/types:span",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/container:inlined_vector",
        "@abseil-cpp//absl/log:check",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":parser",
        "@abseil-cpp//absl/strings",
    ],
)