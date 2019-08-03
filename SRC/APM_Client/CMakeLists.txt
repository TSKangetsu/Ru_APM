aux_source_directory(. LIB_CLIENTSHOW)

add_library(APM_Client ${LIB_CLIENTSHOW})

link_libraries("./SRC/ToolsInclude")