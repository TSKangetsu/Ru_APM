aux_source_directory(. LIB_CAMERAINCLUDE)

add_library( CameraControl ${LIB_CAMERAINCLUDE} )

link_libraries("./SRC/ToolsInclude")