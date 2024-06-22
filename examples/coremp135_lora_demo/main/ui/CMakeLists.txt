SET(SOURCES components/ui_comp_button1.c
    components/ui_comp.c
    screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_lorawan_gateway_on_coremp135_png.c
    images/ui_img_vector_1_png.c
    images/ui_img_cpu_load__png.c
    images/ui_img_ram__png.c
    images/ui_img_cpu_temp__png.c
    images/ui_img_ip___png.c
    images/ui_img_package_png.c
    images/ui_img_downlink__png.c
    images/ui_img_uplink___png.c
    images/ui_img_rectangle_15_png.c
    images/ui_img_frame_3_png.c
    images/ui_img_lora_1_png.c
    images/ui_img_cm4_stack_logo1_png.c
    images/ui_img_vector_2_png.c
    images/ui_img_vector_3_png.c
    images/ui_img_1858829983.c
    fonts/ui_font_mono.c
    fonts/ui_font_mono13.c)

add_library(ui ${SOURCES})
