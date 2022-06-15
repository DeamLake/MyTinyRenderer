set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${curr_dir} Shader_SRCS)

add_library(shader ${Shader_SRCS} "BlingPhongShader.h")