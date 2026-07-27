set(COMPONENT_SRCDIRS
    "src"
)

set(COMPONENT_ADD_INCLUDEDIRS
    "src"
)

set(COMPONENT_REQUIRES
   "arduino"
)

# target_compile_definitions(${COMPONENT_LIB} PRIVATE ESP32 ARDUINO)


register_component()
