# Config enable component3 or not in Kconfig
set(PRIVATE_MODULE_DIR "../../github_source")
if(CONFIG_NNG_ENABLED)

    ################# Add include #################

	list(APPEND ADD_INCLUDE "include")
    ###############################################

    ############## Add source files ###############
	# append_srcs_dir(ADD_SRCS "src")
    list(APPEND ADD_SRCS  "test.c")
    list(APPEND ADD_PRIVATE_INCLUDE "${PRIVATE_MODULE_DIR}/nng/src")
    set(NNG_SRCS_DIRS "${PRIVATE_MODULE_DIR}/nng/src"
                      "${PRIVATE_MODULE_DIR}/nng/src/core"
                      "${PRIVATE_MODULE_DIR}/nng/src/platform/posix"
                      "${PRIVATE_MODULE_DIR}/nng/src/compat/nanomsg"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/protocol/bus0"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/protocol/pair0"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/protocol/pair1"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/protocol/pipeline0"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/protocol/pubsub0"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/protocol/reqrep0"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/protocol/survey0"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/transport/inproc"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/transport/ipc"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/transport/tcp"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/transport/tls"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp/transport/ws"
                      "${PRIVATE_MODULE_DIR}/nng/src/sp"
                      "${PRIVATE_MODULE_DIR}/nng/src/supplemental/base64"
                      "${PRIVATE_MODULE_DIR}/nng/src/supplemental/http"
                      "${PRIVATE_MODULE_DIR}/nng/src/supplemental/sha1"
                      "${PRIVATE_MODULE_DIR}/nng/src/supplemental/tls"
                      "${PRIVATE_MODULE_DIR}/nng/src/supplemental/util"
                      "${PRIVATE_MODULE_DIR}/nng/src/supplemental/websocket"
    )

    append_srcs_dir(ADD_SRCS ${NNG_SRCS_DIRS})

    foreach(item IN ITEMS ${ADD_SRCS})
        string(FIND ${item} "_test.c" found)
        if(found GREATER -1)
            list(REMOVE_ITEM ADD_SRCS "${item}")
        endif()
    endforeach()

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

    list(APPEND ADD_DEFINITIONS_PRIVATE -DNNG_ENABLE_STATS 
                                        -DNNG_HAVE_ABSTRACT_SOCKETS 
                                        -DNNG_HAVE_BACKTRACE=1 
                                        -DNNG_HAVE_BUS0 
                                        -DNNG_HAVE_CLOCK_GETTIME=1 
                                        -DNNG_HAVE_EPOLL=1 
                                        -DNNG_HAVE_EPOLL_CREATE1=1 
                                        -DNNG_HAVE_EVENTFD=1 
                                        -DNNG_HAVE_FLOCK=1 
                                        -DNNG_HAVE_GETRANDOM=1 
                                        -DNNG_HAVE_LANGINFO=1 
                                        -DNNG_HAVE_LIBATOMIC=1 
                                        -DNNG_HAVE_LIBNSL=1 
                                        -DNNG_HAVE_LOCKF=1 
                                        -DNNG_HAVE_MSG_CONTROL=1 
                                        -DNNG_HAVE_PAIR0 
                                        -DNNG_HAVE_PAIR1 
                                        -DNNG_HAVE_PTHREAD_ATFORK_PTHREAD=1 
                                        -DNNG_HAVE_PTHREAD_SETNAME_NP=1 
                                        -DNNG_HAVE_PUB0 
                                        -DNNG_HAVE_PULL0 
                                        -DNNG_HAVE_PUSH0 
                                        -DNNG_HAVE_REP0 
                                        -DNNG_HAVE_REQ0 
                                        -DNNG_HAVE_RESPONDENT0 
                                        -DNNG_HAVE_SEMAPHORE_PTHREAD=1 
                                        -DNNG_HAVE_SOPEERCRED=1 
                                        -DNNG_HAVE_STDATOMIC=1 
                                        -DNNG_HAVE_STRCASECMP=1 
                                        -DNNG_HAVE_STRNCASECMP=1 
                                        -DNNG_HAVE_STRNLEN=1 
                                        -DNNG_HAVE_SUB0 
                                        -DNNG_HAVE_SURVEYOR0 
                                        -DNNG_HAVE_UNIX_SOCKETS=1 
                                        -DNNG_HIDDEN_VISIBILITY 
                                        -DNNG_MAX_EXPIRE_THREADS=8 
                                        -DNNG_MAX_TASKQ_THREADS=16 
                                        -DNNG_PLATFORM_LINUX 
                                        -DNNG_PLATFORM_POSIX 
                                        -DNNG_PRIVATE 
                                        -DNNG_STATIC_LIB 
                                        -DNNG_SUPP_HTTP 
                                        -DNNG_TRANSPORT_INPROC 
                                        -DNNG_TRANSPORT_IPC 
                                        -DNNG_TRANSPORT_TCP 
                                        -DNNG_TRANSPORT_TLS 
                                        -DNNG_TRANSPORT_WS 
                                        -DNNG_USE_EVENTFD 
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
    if(CONFIG_NNG_DYNAMIC)
        register_component(DYNAMIC)
    else()
        register_component()
    endif()
endif()

