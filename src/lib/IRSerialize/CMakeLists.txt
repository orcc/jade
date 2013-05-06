
file(GLOB_RECURSE IRSerialize_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRSerialize/*.h")

add_library (IRSerialize
  IRConstant.h
  IRLinker.cpp
  IRParser.cpp
  IRWriter.cpp
  IRUnwriter.cpp
  ${IRSerialize_HDRS}
)
