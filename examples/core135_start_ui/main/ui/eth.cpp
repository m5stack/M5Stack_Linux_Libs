/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "hv/ifconfig.h"  
#include "ui.h"


void screen_test_eth_event(lv_event_t *e)
{
    char eth_ip_buff[128] = {0};
    lv_obj_set_style_bg_color(ui_Container[6], lv_color_hex(0xADABEF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_textarea_set_text(ui_TextArea[6], "scan eth ip ...\n");
    std::vector<ifconfig_t> ifcs;
    ifconfig(ifcs);
    int ip_count = 0;
    int net_speed[10];
    for (auto& item : ifcs) {
        if(strlen(item.ip) != 0)
        {
            net_speed[ip_count] = -1;
            sprintf(eth_ip_buff, "/sys/class/net/%s/speed", item.name);
            FILE *file = fopen(eth_ip_buff, "r");
            if(file != NULL)
            {
                fscanf(file, "%i", &net_speed[ip_count]);
                fclose(file);
            }
            sprintf(eth_ip_buff, "%s ip: %s %d\n", item.name, item.ip, net_speed[ip_count]);
            lv_textarea_add_text(ui_TextArea[6], eth_ip_buff);
            ip_count ++;
        }
    }
    lv_textarea_add_text(ui_TextArea[6], "scan eth ip over\n");
    if(ip_count == 2)
    {
        for (int i = 0; i < ip_count; i++)
        {
            if(net_speed[i] != 1000)
            {
                // The network port speed test failed.
                lv_obj_set_style_bg_color(ui_Container[6], lv_color_hex(0xFFBA34), LV_PART_MAIN | LV_STATE_DEFAULT);
                return ;
            }
        }
        // Network port detection passed.
        lv_obj_set_style_bg_color(ui_Container[6], lv_color_hex(0xff00), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else
    {
        // Network port detection failed.
        lv_obj_set_style_bg_color(ui_Container[6], lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}
