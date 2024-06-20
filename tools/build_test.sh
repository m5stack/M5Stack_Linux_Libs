:; if [ ! "linux" ] ; then
@echo off
goto :windows_bat_code
exit /b 0
fi

function linux_bash_code
{
package_lists=(axp2101_read
camera
can_test
core135_start_ui
c_periphery_example
ethercat_examples
firebird_demo
framebuffer_OpenCV
hello_world
hv_demo
lcd_hello_world
linux_gpiod
linux_i2c
linux_modbus
linux_rs485
LoRaSender
LovyanGFX_demo
lt8618sxb_mcu_config
lvgl_demo
ncnn_examples
show_logo
socket_demo
)


check_package_existence() {
    local package=$1
}

for item in "${package_lists[@]}"; do
    pushd $item
    scons -j22
    if [ "$?" != "0" ] ; then
        echo "build $item faile!"
        exit
    fi
    scons distclean
    popd

done
} && linux_bash_code && exit

:windows_bat_code
setlocal
echo this is windows, windows code program;
endlocal && exit /b 0



