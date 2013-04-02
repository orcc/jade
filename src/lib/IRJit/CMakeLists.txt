include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})

add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

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
  
if(UNIX)
  set_target_properties(IRJit PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)