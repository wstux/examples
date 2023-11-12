ExecTarget(messenger_client_v2
    SOURCES
        client_main.cpp
    LIBRARIES
        server
)

ExecTarget(messenger_server_v2
    SOURCES
        server_main.cpp
    LIBRARIES
        server
)

