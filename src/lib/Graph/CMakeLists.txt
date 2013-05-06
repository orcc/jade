
file(GLOB_RECURSE Graph_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/Graph/*.h")

add_library (HDAGGraph
  HDAGEdge.cpp
  HDAGGraph.cpp
  HDAGVertex.cpp
  DotWriter.cpp
  SchedulingError.cpp
  ${Graph_HDRS}
)
