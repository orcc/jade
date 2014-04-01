
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
