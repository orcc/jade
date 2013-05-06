
file(GLOB_RECURSE RVCEngine_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/RVCEngine/*.h")

add_library (RVCEngine
  Constant.h
  Decoder.cpp
  RVCEngine.cpp
  ${RVCEngine_HDRS}
)
