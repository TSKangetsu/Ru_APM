find_package(OpenSSL REQUIRED)
aux_source_directory(. MessageController)
add_library(MessageController ${MessageController})
target_link_libraries(MessageController OpenSSL::SSL)