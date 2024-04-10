import os

COMPONENT_LIST = ''

if os.environ.get('CONFIG_ANYANY_ENABLED') is not None:
    COMPONENT_LIST += 'anyany'

if os.environ.get('CONFIG_BACKWARD_CPP_ENABLED') is not None:
    COMPONENT_LIST += ',Backward_cpp'

if os.environ.get('CONFIG_CIMG_COMPONENT_ENABLED') is not None:
    COMPONENT_LIST += ',Cimg'

if os.environ.get('CONFIG_COMPONENT1_ENABLED') is not None:
    COMPONENT_LIST += ',component1'

if os.environ.get('CONFIG_CPPSTATE_ENABLED') is not None:
    COMPONENT_LIST += ',cppstate'

if os.environ.get('CONFIG_DEVICE_DRIVER_ENABLED') is not None:
    COMPONENT_LIST += ',DeviceDriver'

if os.environ.get('CONFIG_EVENTPP_ENABLED') is not None:
    COMPONENT_LIST += ',eventpp'

if os.environ.get('CONFIG_GEAR_LIBS_ENABLED') is not None:
    COMPONENT_LIST += ',gear_libs'

if os.environ.get('CONFIG_LHV_ENABLED') is not None:
    COMPONENT_LIST += ',hv'

if os.environ.get('CONFIG_LVGL_COMPONENT_ENABLED') is not None:
    COMPONENT_LIST += ',lvgl_component'

if os.environ.get('CONFIG_MINICV2_COMPONENT_ENABLED') is not None:
    COMPONENT_LIST += ',minicv2'

if os.environ.get('CONFIG_MONGOOSE_ENABLED') is not None:
    COMPONENT_LIST += ',mongoose'

if os.environ.get('CONFIG_NANOJPEG_ENABLED') is not None:
    COMPONENT_LIST += ',nanojpeg'

if os.environ.get('CONFIG_NANO_LOG_ENABLED') is not None:
    COMPONENT_LIST += ',nanolog'

if os.environ.get('CONFIG_NANOMSG_ENABLED') is not None:
    COMPONENT_LIST += ',nanomsg'

if os.environ.get('CONFIG_NNG_ENABLED') is not None:
    COMPONENT_LIST += ',nng'

if os.environ.get('CONFIG_SIMPLEINI_ENABLED') is not None:
    COMPONENT_LIST += ',simpleini'

if os.environ.get('CONFIG_SINGLE_HEADER_LIBS_ENABLED') is not None:
    COMPONENT_LIST += ',single_header_libs'

if os.environ.get('CONFIG_STB_ENABLED') is not None:
    COMPONENT_LIST += ',stb'

if os.environ.get('CONFIG_TBOX_ENABLED') is not None:
    COMPONENT_LIST += ',tbox'

if os.environ.get('CONFIG_TIMERCPP_ENABLED') is not None:
    COMPONENT_LIST += ',timercpp'

if os.environ.get('CONFIG_TINY_JPEG_ENABLED') is not None:
    COMPONENT_LIST += ',tinyjpeg'

if os.environ.get('CONFIG_TOOLKIT_ENABLED') is not None:
    COMPONENT_LIST += ',toolkit'

if os.environ.get('CONFIG_U8G2_COMPONENT_ENABLED') is not None:
    COMPONENT_LIST += ',u8g2_component'

if os.environ.get('CONFIG_UTILITIES_ENABLED') is not None:
    COMPONENT_LIST += ',utilities'

if os.environ.get('CONFIG_SOEM_ENABLED') is not None:
    COMPONENT_LIST += ',SOEM'

if os.environ.get('CONFIG_C_PERIPHERY_COMPONENT_ENABLED') is not None:
    COMPONENT_LIST += ',c_periphery'







os.environ["COMPONENT_LIST"] = COMPONENT_LIST