
file(GLOB_RECURSE IRMerger_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRMerger/*.h")

add_library (IRMerger
    CheckPinoRules.cpp
    CheckPinoRules.h
    Merger.cpp
    Rational.cpp
    Rational.h
    SuperInstance.cpp
    ${IRMerger_HDRS}
)
