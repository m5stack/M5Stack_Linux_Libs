import json, os
PRIVATE_MODULE_DIR = os.path.join('..', '..', 'github_source')
COMPONENT_INFO = get_build_info()

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

if os.environ.get('CONFIG_UTILITIES_FMT_ENABLED') is not None:
    COMPONENT_INFO["COMPONENT_ADD_INCLUDE"] += ['party/fmt/include']


if os.environ.get('CONFIG_UTILITIES_EASYMSGCPP_ENABLED') is not None:
    COMPONENT_INFO["COMPONENT_ADD_INCLUDE"] += ['party/easymsgcpp/src/include']
    COMPONENT_INFO["COMPONENT_ADD_SRCS"] += append_srcs_dir('party/easymsgcpp/src')

if os.environ.get('CONFIG_UTILITIES_INI_ENABLED') is not None:
    COMPONENT_INFO["COMPONENT_ADD_INCLUDE"] += ['party/ini']
    COMPONENT_INFO["COMPONENT_ADD_SRCS"] += append_srcs_dir('party/ini')

if os.environ.get('CONFIG_UTILITIES_BASE64_ENABLED') is not None:
    COMPONENT_INFO["COMPONENT_ADD_INCLUDE"] += ['party/base64']
    COMPONENT_INFO["COMPONENT_ADD_SRCS"] += append_srcs_dir('party/base64')

############### Add include ###################
COMPONENT_INFO["COMPONENT_ADD_INCLUDE"] += ['include']
COMPONENT_INFO["COMPONENT_ADD_PRIVATE_INCLUDE"] = []
###############################################

############ Add source files #################
COMPONENT_INFO["COMPONENT_ADD_SRCS"].append(append_srcs('test.c'))
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


if os.environ.get('CONFIG_UTILITIES_DYNAMIC') is not None:
    COMPONENT_INFO["COMPONENT_REGISTER_COMPONENT"] = 'sharedlib'
else:
    COMPONENT_INFO["COMPONENT_REGISTER_COMPONENT"] = 'staticlib'

###############################################

os.environ["COMPONENT_INFO"] = json.dumps(COMPONENT_INFO)

    

