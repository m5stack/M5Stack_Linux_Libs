/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "ethercat.h"
#define EC_TIMEOUTMON 500

char IOmap[4096];
OSAL_THREAD_HANDLE thread_ecatcheck;
OSAL_THREAD_HANDLE thread_freash; 
int expectedWKC;
volatile int wkc;
boolean inOP = FALSE;
;
uint8 currentgroup = 0;

OSAL_THREAD_FUNC ecatcheck(void *ptr) {
    int slave;
    (void)ptr; /* Not used */

    while (1) {
        // Check if the system is in operational state and if the working
        // counter is less than expected or if any group requires a state check.
        if (inOP &&
            ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate)) {
            // One or more slaves are not responding
            ec_group[currentgroup].docheckstate = FALSE;
            ec_readstate();  // Read the current state of all slaves

            for (slave = 1; slave <= ec_slavecount; slave++) {
                // Check if the slave belongs to the current group and is not
                // operational
                if ((ec_slave[slave].group == currentgroup) &&
                    (ec_slave[slave].state != EC_STATE_OPERATIONAL)) {
                    ec_group[currentgroup].docheckstate = TRUE;

                    if (ec_slave[slave].state ==
                        (EC_STATE_SAFE_OP + EC_STATE_ERROR)) {
                        // If the slave is in SAFE_OP + ERROR state, attempt to
                        // acknowledge the error
                        printf(
                            "ERROR : slave %d is in SAFE_OP + ERROR, "
                            "attempting ack.\n",
                            slave);
                        ec_slave[slave].state =
                            (EC_STATE_SAFE_OP + EC_STATE_ACK);
                        ec_writestate(slave);

                    } else if (ec_slave[slave].state == EC_STATE_SAFE_OP) {
                        // If the slave is in SAFE_OP state, try to change it to
                        // OPERATIONAL state
                        printf(
                            "WARNING : slave %d is in SAFE_OP, change to "
                            "OPERATIONAL.\n",
                            slave);
                        ec_slave[slave].state = EC_STATE_OPERATIONAL;
                        ec_writestate(slave);

                    } else if (ec_slave[slave].state > EC_STATE_NONE) {
                        // If the slave is in any state other than NONE, attempt
                        // to reconfigure the slave
                        if (ec_reconfig_slave(slave, EC_TIMEOUTMON)) {
                            ec_slave[slave].islost = FALSE;
                            printf("MESSAGE : slave %d reconfigured\n", slave);
                        }
                    } else if (!ec_slave[slave].islost) {
                        // If the slave state is NONE, mark it as lost and check
                        // its state again
                        ec_statecheck(slave, EC_STATE_OPERATIONAL,
                                      EC_TIMEOUTRET);
                        if (ec_slave[slave].state == EC_STATE_NONE) {
                            ec_slave[slave].islost = TRUE;
                            printf("ERROR : slave %d lost\n", slave);
                        }
                    }
                }

                if (ec_slave[slave].islost) {
                    if (ec_slave[slave].state == EC_STATE_NONE) {
                        // Attempt to recover the lost slave
                        if (ec_recover_slave(slave, EC_TIMEOUTMON)) {
                            ec_slave[slave].islost = FALSE;
                            printf("MESSAGE : slave %d recovered\n", slave);
                        }
                    } else {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d found\n", slave);
                    }
                }
            }

            if (!ec_group[currentgroup].docheckstate)
                printf("OK : all slaves resumed OPERATIONAL.\n");
        }
        osal_usleep(10000);  // Sleep for 10 milliseconds before the next check
    }
}

OSAL_THREAD_FUNC ecatfreash(void *ptr) {
    (void)ptr;                 // Unused parameter
    int i, oloop, iloop, chk;  // Variable declarations
    inOP         = FALSE;      // Initialize inOP to FALSE
    char *ifname = "end1";   // Network interface name

    // Initialize EtherCAT on the specified network interface
    if (ec_init(ifname)) {
        printf("ec_init on %s succeeded.\n", ifname);

        // Discover and configure slaves
        if (ec_config_init(FALSE) > 0) {
            printf("%d slaves found and configured.\n", ec_slavecount);

            // Map the process data
            ec_config_map(&IOmap);

            // Configure distributed clocks
            ec_configdc();

            printf("Slaves mapped, state to SAFE_OP.\n");

            // Wait for all slaves to reach SAFE_OP state
            ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);

            // Determine output and input sizes for the first slave
            oloop = ec_slave[0].Obytes;
            if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
            if (oloop > 8) oloop = 8;
            iloop = ec_slave[0].Ibytes;
            if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
            if (iloop > 8) iloop = 8;

            // Print segment information
            printf("segments : %d : %d %d %d %d\n", ec_group[0].nsegments,
                   ec_group[0].IOsegment[0], ec_group[0].IOsegment[1],
                   ec_group[0].IOsegment[2], ec_group[0].IOsegment[3]);

            printf("Request operational state for all slaves\n");

            // Calculate expected working counter
            expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
            printf("Calculated workcounter %d\n", expectedWKC);

            // Request operational state for all slaves
            ec_slave[0].state = EC_STATE_OPERATIONAL;

            // Send initial process data to make outputs in slaves happy
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);

            // Request OP state for all slaves
            ec_writestate(0);
            chk = 200;

            // Wait for all slaves to reach OP state
            do {
                ec_send_processdata();
                ec_receive_processdata(EC_TIMEOUTRET);
                ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
            } while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

            if (ec_slave[0].state == EC_STATE_OPERATIONAL) {
                printf("Operational state reached for all slaves.\n");
                inOP = TRUE;

                // Cyclic loop
                while (1) {
                    ec_send_processdata();
                    wkc = ec_receive_processdata(
                        EC_TIMEOUTRET);  // Receive process data
                    osal_usleep(5000);   // Sleep for 5000 microseconds
                }
                inOP = FALSE;
            } else {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for (i = 1; i <= ec_slavecount; i++) {
                    if (ec_slave[i].state != EC_STATE_OPERATIONAL) {
                        printf(
                            "Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                            i, ec_slave[i].state, ec_slave[i].ALstatuscode,
                            ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }
            printf("\nRequest init state for all slaves\n");

            // Request INIT state for all slaves
            ec_slave[0].state = EC_STATE_INIT;
            ec_writestate(0);
        } else {
            printf("No slaves found!\n");
        }
    }
}

int main(int argc, char *argv[]) {
    // Print initial messages to indicate the start of the program
    printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

    // Create a thread to handle slave error detection in operational state
    osal_thread_create(&thread_ecatcheck, 128000, &ecatcheck, NULL);
    // Create a thread to handle refreshing, possibly to monitor or update slave
    // states
    osal_thread_create(&thread_freash, 128000, &ecatfreash, NULL);

    // Wait for a while to allow the refresh thread to initialize properly
    // This wait is necessary because the ecatfreash thread needs some time to
    // complete its initialization
    while (ec_slave[0].outputs == 0);

    // Infinite loop to handle EtherCAT communication
    while (1) {
        // Uncomment the following line if you want the first slave's output to
        // mirror its input This would result in the slave's LED turning on when
        // a button is pressed ec_slave[0].outputs[0] = ec_slave[0].inputs[0];

        // Set the output of the first slave to 0x0f (turn on certain LEDs)
        ec_slave[0].outputs[0x0000] = 0x0f;
        // Sleep for 1 second
        osal_usleep(1000 * 1000);
        // Set the output of the first slave to 0xf0 (turn on other LEDs)
        ec_slave[0].outputs[0x0000] = 0xf0;
        // Sleep for 1 second
        osal_usleep(1000 * 1000);
    }

    // Close the EtherCAT connection
    ec_close();
    // Print a message indicating the end of the program
    printf("End program %s\n", argv[0]);
    return 0;
}
