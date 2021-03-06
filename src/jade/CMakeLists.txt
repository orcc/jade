
set(EXECUTABLE_OUTPUT_PATH ${JADE_OUTPUT_PATH})

add_executable(Jade
    Jade.cpp
    Console.cpp
    Console.h
)

# Libraries required
target_link_libraries(Jade
    RVCEngine
    ConfigurationEngine
    XDFSerialize
    XCFSerialize
    IRMerger
    Scenario
    IROptimize
    IRSerialize
    RoundRobinScheduler
    IRJit
    IRActor
    IRUtil
    IRCore
    HDAGGraph
    TinyXml
    orcc
    ${LLVM_LIBRARIES}
    ${LLVM_LD_FLAGS}
    ${LLVM_SYSTEM_LIBS}
)

# Configure the SDL version
if(SDL2_FOUND)
    target_link_libraries(Jade ${SDL2_LIBRARY})
else()
    target_link_libraries(Jade ${SDL_LIBRARY})
endif()

install(TARGETS Jade
    RUNTIME DESTINATION bin
)
