
file(GLOB_RECURSE Scheduler_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/Scheduler/*.h")
file(GLOB_RECURSE RoundRobinScheduler_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/RoundRobinScheduler/*.h")

add_library (RoundRobinScheduler
    ActionSchedulerAdder.cpp
    ActionSchedulerAdder.h
    CSDFScheduler.cpp
    CSDFScheduler.h
    DPNScheduler.cpp
    DPNScheduler.h
    QSDFScheduler.cpp
    QSDFScheduler.h
    RoundRobinScheduler.cpp
    Fifo.cpp
    ${Scheduler_HDRS}
    ${RoundRobinScheduler_HDRS}
)
