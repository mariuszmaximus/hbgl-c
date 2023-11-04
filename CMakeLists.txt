cmake_minimum_required(VERSION 3.10.0)
project(HBGL-C VERSION 0.1.0 LANGUAGES C CXX)

aux_source_directory(src SRC_FILES)
aux_source_directory(include SRC_FILES)

add_library(hbgl STATIC ${SRC_FILES})
target_include_directories(hbgl PUBLIC include)
if (WIN32)
    target_link_libraries(hbgl PUBLIC glfw3 opengl32 m  )
else()
    target_link_libraries(hbgl PUBLIC glfw  gl       m  )
endif()

add_subdirectory(examples)
