/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <iostream>
#include "eventpp/eventqueue.h"

typedef enum { 
    EVENT_ONE = 0, 
    EVENT_TWO, 
    EVENT_THREE
} local_event_t;
eventpp::EventQueue<local_event_t, void(void)> event_queue;

void test_1(void) {
    std::cout << "test_1 is run ,will run test_2" << std::endl;
    event_queue.enqueue(EVENT_TWO);
}
void test_2(void) {
    std::cout << "test_2 is run ,will run test_3" << std::endl;
    event_queue.enqueue(EVENT_THREE);
}
void test_3(void) {
    std::cout << "test_2 is run ,will exit" << std::endl;
    exit(0);
}

int main(int argc, char **argv) {
    event_queue.appendListener(EVENT_ONE, test_1);
    event_queue.appendListener(EVENT_TWO, test_2);
    event_queue.appendListener(EVENT_THREE, test_3);
    event_queue.enqueue(EVENT_ONE);
    while (1) {
        event_queue.wait();
        event_queue.process();
    }
    return 0;
}
