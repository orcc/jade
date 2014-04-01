
# Common include directories
# Orcc library
include_directories(orcc/include)

file(GLOB orcc_HDRS "orcc/include/*")

set(runtime_sources
    orcc/src/compare.c
    orcc/src/compareyuv.c
    orcc/src/getopt.c
    orcc/src/source.c
    orcc/src/writer.c
    orcc/src/orcc_util.c
    orcc/src/thread.c
    orcc/src/genetic.c
    orcc/src/fpsPrint.c
)

find_package(SDL2 QUIET)
if(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIR})
    list(APPEND runtime_sources orcc/src/display_sdl2.c)
    message(STATUS "SDL2 used")
elseif(SDL_FOUND)
    include_directories(${SDL_INCLUDE_DIR})
    list(APPEND runtime_sources orcc/src/display_sdl.c)
    message(STATUS "SDL1 used")
endif()

add_library (orcc
    ${runtime_sources}
    ${orcc_HDRS}
)

