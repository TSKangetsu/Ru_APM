aux_source_directory(. LIB_CONFIG)

add_library(ConfigSetter ${LIB_CONFIG})

target_link_libraries(ConfigSetter Tools)