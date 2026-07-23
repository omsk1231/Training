# cmake_minimum_required(VERSION 3.5)
# idf_component_register(SRC "src/PubSubClient.cpp" INCLUDE_DIRS "src")

# # set(EXTRA_COMPONENT_REQUIRES ../arduino/)

# project(PubSubClient)
set(COMPONENT_SRCDIRS
    "src"
)

set(COMPONENT_ADD_INCLUDEDIRS
    "src"
)

set(COMPONENT_REQUIRES
    "arduino"
)

register_component()