# cmake_minimum_required(VERSION 3.5)

# # set(EXTRA_COMPONENT_REQUIRES ../arduino/)
# idf_component_register(SRC "AHT20.cpp" INCLUDE_DIRS "src")

# project(AHT20)
set(COMPONENT_SRCDIRS
    "src"
)

set(COMPONENT_ADD_INCLUDEDIRS
    "src"
)

set(COMPONENT_REQUIRES
    "arduino" "Adafruit_Sensor"
)

register_component()

