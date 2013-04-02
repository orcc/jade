add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})

file(GLOB_RECURSE IRCore_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRCore/*.h")

add_library (IRCore
	Action.cpp 
	ActionScheduler.cpp 
	Actor.cpp
	Connection.cpp
	Entity.cpp
	Expression.cpp
	FSM.cpp
	Instance.cpp
	Network.cpp
	Package.cpp
	Pattern.cpp
	Port.cpp
	Procedure.cpp
	${IRCore_HDRS}
 )
 
if(UNIX)
  set_target_properties(IRCore PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
