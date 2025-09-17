/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include <signal.h>
#include <stdio.h>

class signal_handler {
public:
    signal_handler() {
        signal(SIGINT, handler);
    }

    bool is_quit() const {
        return m_quit;
    }

    static void handler(int s) {
        printf("\n====================== caught signal: %d ======================\n", s);
        m_quit = 1;
    }

protected:
    signal_handler(const signal_handler&) = delete;
    signal_handler& operator=(const signal_handler&) = delete;

private:
    static bool m_quit;
};

bool signal_handler::m_quit = false;
