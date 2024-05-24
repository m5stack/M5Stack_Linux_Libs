#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>

int main(void) {
    modbus_t *ctx;
    uint16_t tab_reg[64];

    // Create a new Modbus RTU context
    ctx = modbus_new_rtu("/dev/ttySTM3", 115200, 'N', 8, 1);
    if (ctx == NULL) {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        return -1;
    }

    // Set the slave address
    modbus_set_slave(ctx, 1);

    // Open the serial port
    if (modbus_connect(ctx) == -1) {
        // fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    // Read registers (e.g., read 10 registers starting from address 0)
    int rc = modbus_read_registers(ctx, 0, 10, tab_reg);
    if (rc == -1) {
        // fprintf(stderr, "Failed to read registers: %s\n", modbus_strerror(errno));
        modbus_close(ctx);
        modbus_free(ctx);
        return -1;
    }

    // Print the values of the registers read
    for (int i = 0; i < rc; i++) {
        printf("reg[%d]=%d\n", i, tab_reg[i]);
    }

    // Write register (e.g., write the value 123 to register address 0)
    rc = modbus_write_register(ctx, 0, 123);
    if (rc == -1) {
        // fprintf(stderr, "Failed to write register: %s\n", modbus_strerror(errno));
        modbus_close(ctx);
        modbus_free(ctx);
        return -1;
    }

    // Close the connection and free the context
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
