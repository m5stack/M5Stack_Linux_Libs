# Config enable component3 or not in Kconfig
set(PRIVATE_MODULE_DIR "${SDK_PATH}/github_source")
if(CONFIG_DEVICE_DRIVER_ENABLED)

    list(APPEND ADD_INCLUDE "party")

    if(CONFIG_M5DEVICE_SH1107)
        append_srcs_dir(ADD_SRCS "party/m5_sh1107")
    endif()

    if(CONFIG_DEVICE_UART_ENABLED)
        append_srcs_dir(ADD_SRCS "party/linux_uart")
    endif()

    if(CONFIG_DEVICE_I2C_ENABLED)
        append_srcs_dir(ADD_SRCS "party/linux_i2c")
    endif()

    if(CONFIG_DEVICE_SPI_ENABLED)
        append_srcs_dir(ADD_SRCS "party/linux_spi")
    endif()

    if(CONFIG_DEVICE_FRAMEBUFFER_ENABLED)
        append_srcs_dir(ADD_SRCS "party/framebuffer")
    endif()

    if(CONFIG_DEVICE_PTMX_ENABLED)
        append_srcs_dir(ADD_SRCS "party/ptmx")
    endif()


    ################# Add include #################

	# list(APPEND ADD_INCLUDE "include")

    # list(APPEND ADD_PRIVATE_INCLUDE ""
    # )
    ###############################################

    ############## Add source files ###############

	append_srcs_dir(ADD_SRCS "test.c")

    # list(APPEND ADD_SRCS  "log.c/src/superlog.c"
    #     )
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
    list(APPEND ADD_REQUIREMENTS pthread)
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
	# list(APPEND ADD_LINKOPTIONS_PRIVATE -w)
    #### Add compile option for this component
    #### and components denpend on this component
    list(APPEND ADD_DEFINITIONS_PRIVATE -w)
    # list(APPEND ADD_DEFINITIONS -w)
    ###############################################

    ############ Add static libs ##################
    #### Update parent's variables like CMAKE_C_LINK_FLAGS
    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group lib/libtest.a -ltest2 -Wl,--end-group" PARENT_SCOPE)
    ###############################################

    # register component, DYNAMIC or SHARED flags will make component compiled to dynamic(shared) lib
    if(CONFIG_DEVICE_DRIVER_DYNAMIC)
        register_component(DYNAMIC)
    else()
        register_component()
    endif()
endif()

