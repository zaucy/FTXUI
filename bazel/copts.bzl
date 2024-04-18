load("@bazel_skylib//lib:selects.bzl", "selects")

copts = selects.with_or({
    ("//conditions:default"): [
        "-std=c++20",
    ],
    ("@rules_cc//cc/compiler:msvc-cl", "@rules_cc//cc/compiler:clang-cl"): [
        "/std:c++20",
        "/permissive-",
        "/utf-8",
        "/wd4267",
        "/wd4244",
    ],
})
