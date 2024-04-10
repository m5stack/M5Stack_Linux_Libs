import json, os
COMPONENT_INFO = {}

# COMPONENT_ADD_INCLUDE = ''
# COMPONENT_ADD_PRIVATE_INCLUDE = ''
# COMPONENT_ADD_SRCS = ''
# COMPONENT_ADD_ASM_SRCS = ''
# COMPONENT_ADD_DEFINITIONS = ''
# COMPONENT_ADD_DEFINITIONS_PRIVATE = ''
# COMPONENT_ADD_REQUIREMENTS = ''
# COMPONENT_ADD_LINKOPTIONS_PRIVATE = ''
# COMPONENT_ADD_STATIC_LIB = ''
# COMPONENT_ADD_DYNAMIC_LIB = ''
# COMPONENT_ADD_C_FLAGS = ''
# COMPONENT_ADD_CXX_FLAGS = ''
# COMPONENT_ADD_C_LINK_FLAGS = ''
# COMPONENT_ADD_CXX_LINK_FLAGS = ''


# COMPONENT_C_FLAGS = ''
# COMPONENT_CXX_FLAGS = ''
# COMPONENT_C_LINK_FLAGS = ''
# COMPONENT_CXX_LINK_FLAGS = ''
COMPONENT_INFO["COMPONENT_ADD_SRCS"] = []

if os.environ.get('CONFIG_M5DEVICE_SH1107') is not None:
    COMPONENT_INFO["COMPONENT_ADD_SRCS"] += append_srcs_dir('party/m5_sh1107')

if os.environ.get('CONFIG_DEVICE_UART_ENABLED') is not None:
    COMPONENT_INFO["COMPONENT_ADD_SRCS"] += append_srcs_dir('party/linux_uart')

if os.environ.get('CONFIG_DEVICE_I2C_ENABLED') is not None:
    COMPONENT_INFO["COMPONENT_ADD_SRCS"] += append_srcs_dir('party/linux_i2c')

if os.environ.get('CONFIG_DEVICE_SPI_ENABLED') is not None:
    COMPONENT_INFO["COMPONENT_ADD_SRCS"] += append_srcs_dir('party/linux_spi')

if os.environ.get('CONFIG_DEVICE_FRAMEBUFFER_ENABLED') is not None:
    COMPONENT_INFO["COMPONENT_ADD_SRCS"] += append_srcs_dir('party/framebuffer')

if os.environ.get('CONFIG_DEVICE_PTMX_ENABLED') is not None:
    COMPONENT_INFO["COMPONENT_ADD_SRCS"] += append_srcs_dir('party/ptmx')



############### Add include ###################
COMPONENT_INFO["COMPONENT_ADD_INCLUDE"] = ['party']
COMPONENT_INFO["COMPONENT_ADD_PRIVATE_INCLUDE"] = []
###############################################

############ Add source files #################

COMPONENT_INFO["COMPONENT_ADD_ASM_SRCS"] = []

###############################################

###### Add required/dependent components ######
COMPONENT_INFO["COMPONENT_ADD_REQUIREMENTS"] = []

###############################################

###### Add link search path for requirements/libs ######
###############################################

############ Add static libs ##################
COMPONENT_INFO["COMPONENT_ADD_STATIC_LIB"] = []
###############################################

############ Add dynamic libs ##################
COMPONENT_INFO["COMPONENT_ADD_DYNAMIC_LIB"] = []
###############################################


#### Add compile option for this component ####
#### Just for this component, won't affect other 
#### modules, including component that depend 
#### on this component

COMPONENT_INFO["COMPONENT_ADD_DEFINITIONS"] = []
COMPONENT_INFO["COMPONENT_ADD_DEFINITIONS_PRIVATE"] = []
COMPONENT_INFO["COMPONENT_ADD_LINK_FLAGS"] = []
COMPONENT_INFO["COMPONENT_ADD_LINK_FLAGS_PRIVATE"] = []


if os.environ.get('DEVICE_DRIVER_DYNAMIC') is not None:
    COMPONENT_INFO["COMPONENT_REGISTER_COMPONENT"] = 'sharedlib'
else:
    COMPONENT_INFO["COMPONENT_REGISTER_COMPONENT"] = 'staticlib'

###############################################

os.environ["COMPONENT_INFO"] = json.dumps(COMPONENT_INFO)

    

