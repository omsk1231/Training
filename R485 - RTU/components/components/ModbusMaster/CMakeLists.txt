# cmake_minimum_required(VERSION 3.5)

# # set(EXTRA_COMPONENT_REQUIRES ../arduino/)
# idf_component_register(SRC "ModbusMaster.cpp" INCLUDE_DIRS "src")

# project(ModbusMaster)
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

