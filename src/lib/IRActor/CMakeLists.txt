
file(GLOB_RECURSE IRActor_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRActor/*.h")

add_library (IRActor
    BroadcastActor.cpp
    BroadcastAdder.cpp
    ${IRActor_HDRS}
)
