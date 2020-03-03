aux_source_directory(. APMController)

find_library(mill mill)

add_library(APMController ${APMController})

target_link_libraries( APMController ${mill})