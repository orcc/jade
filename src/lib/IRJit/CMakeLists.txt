
file(GLOB_RECURSE IRJit_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRJit/*.h")

add_library (IRJit
  LLVMArmFix.cpp 
  LLVMExecution.cpp 
  LLVMOptimizer.cpp 
  LLVMParser.cpp 
  LLVMUtility.cpp 
  LLVMWriter.cpp 
  NativeDecl.h
  ${IRJit_HDRS}
)