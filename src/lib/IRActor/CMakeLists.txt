include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})

add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

file(GLOB_RECURSE IRActor_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRActor/*.h")

add_library (IRActor
  BroadcastActor.cpp
  BroadcastAdder.cpp
  ${IRActor_HDRS}
)

if(UNIX)
  set_target_properties(IRActor PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
