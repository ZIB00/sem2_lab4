add_library(LazySequence SHARED
    declaration/
    implementation/
)

target_include_directories(LazySequence PUBLIC Sequences)
