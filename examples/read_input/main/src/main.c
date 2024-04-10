#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

int main() {
    const char *device_path = "/dev/input/event0"; // 替换为实际的输入设备路径
    int fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open input device");
        return 1;
    }

    struct input_event ev;
    while (1) {
        if (read(fd, &ev, sizeof(struct input_event)) == sizeof(struct input_event)) {
            if (ev.type == EV_ABS && (ev.code == ABS_X || ev.code == ABS_Y)) {
                if (ev.code == ABS_X) {
                    printf("X: %d\n", ev.value);
                } else if (ev.code == ABS_Y) {
                    printf("Y: %d\n", ev.value);
                }
            } else if (ev.type == EV_KEY) {
                if (ev.code == BTN_TOUCH) {
                    printf("Touch state: %s\n", ev.value ? "pressed" : "released");
                }
            }
        }
    }

    close(fd);
    return 0;
}
