AUX_SOURCE_DIRECTORY(valonGui/src DIR_SRCS)

list(APPEND SRC
    ${DIR_SRCS}
    valonGui/src/fonts/vlGui_fonts.c
)

idf_component_register(SRCS "${SRC}"
                INCLUDE_DIRS "valonGui/inc"
                REQUIRES)

