# Config enable component3 or not in Kconfig
set(PRIVATE_MODULE_DIR "../../github_source")
if(CONFIG_LVGL_COMPONENT_ENABLED)

    # # message(WARNING "source path: ${LV_CONF_INCLUDE_SIMPLE}-------")


    list(APPEND ADD_INCLUDE "${PRIVATE_MODULE_DIR}/lvgl")


    if(CONFIG_LVGL_COMPONENT_LVGL)
        set(LVGL_PATH "${PRIVATE_MODULE_DIR}/lvgl/lvgl")
        file(GLOB_RECURSE LVGL_SOURCE_FILES "${LVGL_PATH}/src/*.c"
                                            "${LVGL_PATH}/demos/*.c"
                                            "${LVGL_PATH}/examples/*.c"
        )
        list(APPEND ADD_SRCS  "${LVGL_SOURCE_FILES}")
    endif()

    if(CONFIG_LVGL_COMPONENT_LV_DRIVERS)
        set(LV_DRIVERS_DIR_NAME "${PRIVATE_MODULE_DIR}/lvgl/lv_drivers")
        file(GLOB LV_DRIVER_SOURCE_FILES "${LV_DRIVERS_DIR_NAME}/*.c"
                                        "${LV_DRIVERS_DIR_NAME}/wayland/*.c"
                                        "${LV_DRIVERS_DIR_NAME}/indev/*.c"
                                        "${LV_DRIVERS_DIR_NAME}/gtkdrv/*.c"
                                        "${LV_DRIVERS_DIR_NAME}/display/*.c"
                                        "${LV_DRIVERS_DIR_NAME}/sdl/*.c"
        )
        list(APPEND ADD_SRCS  "${LV_DRIVER_SOURCE_FILES}")
    endif()


    ################# Add include #################

	list(APPEND ADD_INCLUDE "include")

    # ###############################################

    # ############## Add source files ###############


    list(APPEND ADD_SRCS  "test.c")
    # FILE(GLOB_RECURSE EXTRA_SRC  "src/*.c")
    # FILE(GLOB EXTRA_SRC  "src/*.c")
    # list(APPEND ADD_SRCS  ${EXTRA_SRC})
    # aux_source_directory(src ADD_SRCS)  # collect all source file in src dir, will set var ADD_SRCS
    # append_srcs_dir(ADD_SRCS "src")     # append source file in src dir to var ADD_SRCS
    # list(REMOVE_ITEM COMPONENT_SRCS "src/test.c")
    # set(ADD_ASM_SRCS "src/asm.S")
    # list(APPEND ADD_SRCS ${ADD_ASM_SRCS})
    # SET_PROPERTY(SOURCE ${ADD_ASM_SRCS} PROPERTY LANGUAGE C) # set .S  ASM file as C language
    # SET_SOURCE_FILES_PROPERTIES(${ADD_ASM_SRCS} PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -D BBBBB")
    ###############################################


    ###### Add required/dependent components ######
    # list(APPEND ADD_REQUIREMENTS lvgl_example)
    ###############################################

    ###### Add link search path for requirements/libs ######
    # list(APPEND ADD_LINK_SEARCH_PATH "${CONFIG_TOOLCHAIN_PATH}/lib")
    # list(APPEND ADD_REQUIREMENTS pthread m)  # add system libs, pthread and math lib for example here
    # set (OpenCV_DIR opencv/lib/cmake/opencv4)
    # find_package(OpenCV REQUIRED)
    ###############################################

    ############ Add static libs ##################
    # list(APPEND ADD_STATIC_LIB "lib/libtest.a")
    ###############################################

    ############ Add dynamic libs ##################
    # list(APPEND ADD_DYNAMIC_LIB "lib/test1.so"
    #                             "lib/test2.so"
    # )
    ###############################################

    #### Add compile option for this component ####
    #### Just for this component, won't affect other 
    #### modules, including component that depend 
    #### on this component
	# list(APPEND ADD_LINKOPTIONS_PRIVATE -DLV_CONF_INCLUDE_SIMPLE)
    #### Add compile option for this component
    #### and components denpend on this component
    # list(APPEND ADD_DEFINITIONS_PRIVATE -Wall )
    # list(APPEND ADD_DEFINITIONS -w)
    ###############################################

    ############ Add static libs ##################
    #### Update parent's variables like CMAKE_C_LINK_FLAGS
    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group lib/libtest.a -ltest2 -Wl,--end-group" PARENT_SCOPE)
    ###############################################

    # register component, DYNAMIC or SHARED flags will make component compiled to dynamic(shared) lib
    if(CONFIG_LVGL_COMPONENT_DYNAMIC)
        register_component(DYNAMIC)
    else()
        register_component()
    endif()
endif()

