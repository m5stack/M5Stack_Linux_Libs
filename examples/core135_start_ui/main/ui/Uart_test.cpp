/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
// #include "hv/ifconfig.h"
#include <sys/types.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <thread>

#include "linux_uart/linux_uart.h"
#include "ui.h"

extern pthread_mutex_t device_thread_mutex;

std::string execCmd(const char *cmd) {
    FILE *pp = popen(cmd, "r");
    char tmp[1024];
    std::string result;
    while (fgets(tmp, sizeof(tmp), pp) != NULL) result += tmp;
    pclose(pp);
    return result;
}

extern int page_lock;

class Uart_test {
   private:
    std::thread *ther;
    int ther_flage = 0;

   public:
    Uart_test() { ther = NULL; }
    void start() {
        char buffer[128];
        std::string devstr;
        lv_textarea_add_text(ui_TextArea[10], "scan uart device ...\n");

        std::string out = execCmd("ls /dev/ttyUSB* 2>/dev/null");
        if (out.length() > 0) {
            int index = out.find("/dev/ttyUSB");
            int _exit = 1;
            if (index != std::string::npos) {
                for (int i = index; (i < out.length()) && _exit; i++) {
                    switch (out[i]) {
                        case 'a' ... 'z':
                        case 'A' ... 'Z':
                        case '0' ... '9':
                        case '_':
                        case '/':
                            devstr.push_back(out[i]);
                            break;
                        default:
                            _exit = 0;
                            break;
                    }
                }
                ther_flage = 1;
                sprintf(buffer, "find device %s\n", devstr.c_str());
                lv_textarea_add_text(ui_TextArea[10], buffer);
                ther = new std::thread(
                    std::bind(&Uart_test::cmd_loop, this, devstr));
                return;
            }
            index = out.find("/dev/ttyACM");
            _exit = 1;
            if (index != std::string::npos) {
                for (int i = index; (i < out.length()) && _exit; i++) {
                    switch (out[i]) {
                        case 'a' ... 'z':
                        case 'A' ... 'Z':
                        case '0' ... '9':
                        case '_':
                        case '/':
                            devstr.push_back(out[i]);
                            break;
                        default:
                            _exit = 0;
                            break;
                    }
                }
                ther_flage = 1;
                sprintf(buffer, "find device %s\n", devstr.c_str());
                lv_textarea_add_text(ui_TextArea[10], buffer);
                ther = new std::thread(
                    std::bind(&Uart_test::cmd_loop, this, devstr));
                return;
            }
        }
        lv_textarea_add_text(ui_TextArea[10], "not find device\n");
        lv_obj_set_style_bg_color(ui_Container[10], lv_color_hex(0xFF0000),
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
        page_lock = -1;
        lv_obj_clear_state(ui_Button[11], LV_STATE_DISABLED);
        delet_self();
    }
    void cmd_loop(const std::string &devstr) {
        char buffer[128];
        int size;
        uart_t par  = {.baud       = 115200,
                       .data_bits  = 8,
                       .stop_bits  = 1,
                       .parity     = 'n',
                       .wait_flage = 1};
        int _serial = linux_uart_init((char *)devstr.c_str(), &par);
        if (_serial <= 0) {
            pthread_mutex_lock(&device_thread_mutex);
            sprintf(buffer, "open %s faile\n", devstr.c_str());
            lv_textarea_add_text(ui_TextArea[10], buffer);
            page_lock = -1;
            lv_obj_clear_state(ui_Button[11], LV_STATE_DISABLED);
            pthread_mutex_unlock(&device_thread_mutex);
            return;
        }
        char data;
        size = 1;
        // printf("----------------1\n");
        while (size > 0) {
            size = linux_uart_read(_serial, 128, buffer);
        }
        // printf("----------------2\n");
        size = sprintf(buffer, "\r");
        linux_uart_write(_serial, size, buffer);
        size           = 0;
        uint64_t start = p___millis();
        // printf("----------------3\n");
        int flage = 0;
        while (p___millis() < start + 2000) {
            size = linux_uart_read(_serial, 128, buffer);
            if (size > 0) {
                flage = 1;
                pthread_mutex_lock(&device_thread_mutex);
                lv_textarea_add_text(ui_TextArea[10], buffer);
                pthread_mutex_unlock(&device_thread_mutex);
            }
        }
        if (flage) {
            pthread_mutex_lock(&device_thread_mutex);
            lv_obj_set_style_bg_color(ui_Container[10], lv_color_hex(0xFF00),
                                      LV_PART_MAIN | LV_STATE_DEFAULT);
            page_lock = -1;
            lv_obj_clear_state(ui_Button[11], LV_STATE_DISABLED);
            pthread_mutex_unlock(&device_thread_mutex);
            linux_uart_deinit(_serial);
            delet_self();
            return;
        }

        // printf("----------------4\n");
        pthread_mutex_lock(&device_thread_mutex);
        lv_textarea_add_text(ui_TextArea[10], "not reace msg\n");
        lv_obj_set_style_bg_color(ui_Container[10], lv_color_hex(0xFF0000),
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
        page_lock = -1;
        lv_obj_clear_state(ui_Button[11], LV_STATE_DISABLED);
        pthread_mutex_unlock(&device_thread_mutex);
        linux_uart_deinit(_serial);
        delet_self();
    }
    void delet_self() { delete this; }
};

void screen_test_com_event(lv_event_t *e) {
    lv_textarea_set_text(ui_TextArea[10], "");
    lv_obj_set_style_bg_color(ui_Container[10], lv_color_hex(0xD9D9D9),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_state(ui_Button[11], LV_STATE_DISABLED);
    page_lock       = 10;
    Uart_test *test = new Uart_test();
    test->start();
}