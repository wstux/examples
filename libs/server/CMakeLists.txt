LibTarget(server STATIC
    HEADERS
        client.h
        server.h
        transport.h
        details/utils.h
    SOURCES
        details/client.cpp
        details/server.cpp
    INCLUDE_DIR libs
)

