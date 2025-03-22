file(GLOB_RECURSE COMPONENT_SRCS "src/*.c")

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "include"
    REQUIRES "driver" "espressif__esp-sr"
)