set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${curr_dir} Core_SRCS)

add_library(core ${Core_SRCS})