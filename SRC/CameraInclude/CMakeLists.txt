aux_source_directory(. LIB_CAMERAINCLUDE)

add_library( CameraControl ${LIB_CAMERAINCLUDE} )

target_link_libraries(CameraControl Tools)
target_link_libraries(CameraControl ConfigSetter)