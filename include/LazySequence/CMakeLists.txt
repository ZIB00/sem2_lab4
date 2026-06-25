add_library(LazySequence SHARED
    ActionChangeOrdinal.hpp
    Generator.hpp
    LazySequence.hpp
)

target_include_directories(LazySequence PUBLIC Sequences)
