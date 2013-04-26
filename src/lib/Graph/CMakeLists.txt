include_directories(${JADE_MAIN_INCLUDE_DIR})

add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

file(GLOB_RECURSE Graph_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/Graph/*.h")

add_library (HDAGGraph
  HDAGEdge.cpp
  HDAGGraph.cpp
  HDAGVertex.cpp
  DotWriter.cpp
  SchedulingError.cpp
  ${Graph_HDRS}
)
 
if(UNIX)
  set_target_properties(HDAGGraph PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
