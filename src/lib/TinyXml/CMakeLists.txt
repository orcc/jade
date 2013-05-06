
file(GLOB_RECURSE TinyXml_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/TinyXml/*.h")

add_library (TinyXml
  tinystr.cpp
  tinyxml.cpp
  tinyxmlerror.cpp
  tinyxmlparser.cpp
  ${TinyXml_HDRS}
)
