
add_executable(main main.c)
target_link_libraries(main PRIVATE hbgl)

add_executable(image image.c)
target_link_libraries(image PRIVATE hbgl)

add_executable(text_rand image.c)
target_link_libraries(text_rand PRIVATE hbgl)

add_executable(text image.c)
target_link_libraries(text PRIVATE hbgl)
