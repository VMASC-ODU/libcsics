
add_library(core INTERFACE)

add_library(CSICS::core ALIAS core)

if (CSICS_BUILD_QUEUE)
    add_library(queue STATIC queue/SPSCQueue.cpp)
    target_include_directories(queue PUBLIC ${INCLUDE_DIR})
    add_library(CSICS::queue ALIAS queue)
endif()



