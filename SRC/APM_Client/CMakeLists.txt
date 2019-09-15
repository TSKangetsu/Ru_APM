aux_source_directory(. LIB_CLIENTSHOW)

add_library(APM_Client ${LIB_CLIENTSHOW})

target_link_libraries(APM_Client Tools)