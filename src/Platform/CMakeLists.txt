set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${curr_dir} Platform_SRCS)

add_library(platform ${Platform_SRCS} "Buffer.h"  "micros.h")