set(SOURCES)

if (UNIX)
    list(APPEND SOURCES
        stream/platform/unix/TCPEndpointUnix.cpp
    )
endif()

add_library(net STATIC
    ${SOURCES}
)
target_include_directories(net PUBLIC ${INCLUDE_DIR})

