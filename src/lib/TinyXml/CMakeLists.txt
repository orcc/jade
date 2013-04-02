include_directories(${JADE_MAIN_INCLUDE_DIR})

file(GLOB_RECURSE TinyXml_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/TinyXml/*.h")

add_library (TinyXml
  tinystr.cpp
  tinyxml.cpp
  tinyxmlerror.cpp
  tinyxmlparser.cpp
  ${TinyXml_HDRS}
)
  
if(UNIX)
  set_target_properties(TinyXml PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
