

if(CONFIG_COMPONENT_IMLIB_ENABLE)

list(APPEND ADD_INCLUDE "include")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../maix_smart/include")
add_definitions(-DIMLIB_CONFIG_H_FILE="costom_imlib_config.h")

append_srcs_dir(ADD_SRCS "src")
add_definitions("-lm")
############ Add static libs ##################
# list(APPEND ADD_STATIC_LIB "core/lib/libmaix_nn.a")
###############################################

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libmaix/include)

############ Add dynamic libs ##################

###############################################

register_component(DYNAMIC)
endif()