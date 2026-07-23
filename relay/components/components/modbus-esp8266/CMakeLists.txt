set(COMPONENT_ADD_INCLUDEDIRS src)
set(COMPONENT_SRCS "src/Modbus.cpp" "src/ModbusRTU.cpp")
set(COMPONENT_PRIV_REQUIRES arduino)
register_component()

# cmake_minimum_required(VERSION 3.5)
# idf_component_register(SRC "src/*.cpp" INCLUDE_DIRS "src")

# set(EXTRA_COMPONENT_REQUIRES ../arduino/)

# project(modbus-esp8266)

