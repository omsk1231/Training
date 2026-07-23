# cmake_minimum_required(VERSION 3.5)
# idf_component_register(SRC "src/*.cpp" INCLUDE_DIRS "src")

# set(EXTRA_COMPONENT_REQUIRES ../arduino/)

# project(ESPAsyncWebServer)
set(COMPONENT_SRCDIRS
    "src"
)

set(COMPONENT_ADD_INCLUDEDIRS
    "src"
)

set(COMPONENT_REQUIRES
    "arduino"
    "AsyncTCP"
)

register_component()

target_compile_definitions(${COMPONENT_TARGET} PUBLIC -DESP32)
target_compile_options(${COMPONENT_TARGET} PRIVATE -fno-rtti)