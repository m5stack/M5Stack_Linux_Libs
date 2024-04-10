# Config enable component3 or not in Kconfig
set(PRIVATE_MODULE_DIR "../../github_source")
if(CONFIG_NANOMSG_ENABLED)

    ################# Add include #################

	list(APPEND ADD_INCLUDE "include")
    ###############################################

    ############## Add source files ###############
	# append_srcs_dir(ADD_SRCS "src")
    list(APPEND ADD_SRCS  "test.c")
    
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/nanomsg/src"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/aio"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/core"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/devices"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/protocols/bus"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/protocols/pair"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/protocols/pipeline"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/protocols/pubsub"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/protocols/reqrep"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/protocols/survey"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/protocols/utils"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/transports/inproc"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/transports/ipc"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/transports/tcp"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/transports/utils"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/transports/ws"
                              "${PRIVATE_MODULE_DIR}/nanomsg/src/utils"
                              )

    # list(APPEND ADD_SRCS  "log.c/src/superlog.c"
    #     )
    # FILE(GLOB_RECURSE EXTRA_SRC  "src/*.c")
    # FILE(GLOB EXTRA_SRC  "src/*.c")
    # list(APPEND ADD_SRCS  ${EXTRA_SRC})
    # aux_source_directory(src ADD_SRCS)  # collect all source file in src dir, will set var ADD_SRCS
    # append_srcs_dir(ADD_SRCS "src")     # append source file in src dir to var ADD_SRCS
    # list(REMOVE_ITEM ADD_SRCS "*.inc")
    # set(ADD_ASM_SRCS "src/asm.S")
    # list(APPEND ADD_SRCS ${ADD_ASM_SRCS})
    # SET_PROPERTY(SOURCE ${ADD_ASM_SRCS} PROPERTY LANGUAGE C) # set .S  ASM file as C language
    # SET_SOURCE_FILES_PROPERTIES(${ADD_ASM_SRCS} PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -D BBBBB")
    ###############################################


    ###### Add required/dependent components ######
    # list(APPEND ADD_REQUIREMENTS component1)
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
    # list(APPEND ADD_DEFINITIONS_PRIVATE -w)
    # list(APPEND ADD_DEFINITIONS -w)
    list(APPEND ADD_DEFINITIONS_PRIVATE -DNN_HAVE_ACCEPT4=1 
                                        -DNN_HAVE_BACKTRACE=1 
                                        -DNN_HAVE_CLOCK_GETTIME=1 
                                        -DNN_HAVE_CLOCK_MONOTONIC=1 
                                        -DNN_HAVE_EPOLL=1 
                                        -DNN_HAVE_EVENTFD=1 
                                        -DNN_HAVE_GCC_ATOMIC_BUILTINS 
                                        -DNN_HAVE_GETADDRINFO_A=1 
                                        -DNN_HAVE_LIBNSL=1 
                                        -DNN_HAVE_LINUX 
                                        -DNN_HAVE_MSG_CONTROL=1 
                                        -DNN_HAVE_PIPE2=1 
                                        -DNN_HAVE_PIPE=1 
                                        -DNN_HAVE_POLL=1 
                                        -DNN_HAVE_SEMAPHORE 
                                        -DNN_HAVE_SEMAPHORE_PTHREAD=1 
                                        -DNN_HAVE_SEMAPHORE_RT=1 
                                        -DNN_HAVE_SOCKETPAIR=1 
                                        -DNN_HAVE_UNIX_SOCKETS=1 
                                        -DNN_MAX_SOCKETS=512 
                                        -DNN_SHARED_LIB 
                                        -DNN_USE_EPOLL 
                                        -DNN_USE_EVENTFD 
                                        -D_GNU_SOURCE 
                                        -D_POSIX_PTHREAD_SEMANTICS 
                                        -D_REENTRANT 
                                        -D_THREAD_SAFE
                                        )

    ###############################################

    ############ Add static libs ##################
    #### Update parent's variables like CMAKE_C_LINK_FLAGS
    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group lib/libtest.a -ltest2 -Wl,--end-group" PARENT_SCOPE)
    ###############################################

    # register component, DYNAMIC or SHARED flags will make component compiled to dynamic(shared) lib
    if(CONFIG_NANOMSG_DYNAMIC)
        register_component(DYNAMIC)
    else()
        register_component()
    endif()
endif()

