# Linux Debugging Tools

## I2C

In Linux systems, I2C (Inter-Integrated Circuit) is a commonly used bus protocol for connecting multiple low-speed peripherals. To debug and test I2C devices, Linux provides some handy I2C debugging tools, mainly included in the `i2c-tools` package.

### `i2c-tools` Package

`i2c-tools` is a set of convenient tools for interacting with and debugging I2C devices. This package includes some command-line tools that can be used to scan the I2C bus, communicate with I2C devices, and more.

#### Installing `i2c-tools`

In most Linux distributions, you can install `i2c-tools` using the package manager. For example:

- On Debian/Ubuntu:
  ```sh
  sudo apt-get update
  sudo apt-get install i2c-tools
  ```

- On Fedora:
  ```sh
  sudo dnf install i2c-tools
  ```

- On Arch Linux:
  ```sh
  sudo pacman -S i2c-tools
  ```

### Main Tools and Usage

The `i2c-tools` package includes the following main tools:

#### 1. `i2cdetect`
`i2cdetect` is used to scan the I2C bus and list all connected I2C devices and their addresses.

- Scan I2C bus 0:
  ```sh
  sudo i2cdetect -y 0
  ```

- Scan I2C bus 1:
  ```sh
  sudo i2cdetect -y 1
  ```

#### 2. `i2cdump`
`i2cdump` reads and displays the register contents of an I2C device.

- Dump registers of a device at I2C address 0x50 on I2C bus 1:
  ```sh
  sudo i2cdump -y 1 0x50
  ```

#### 3. `i2cget`
`i2cget` reads the value of a single register from an I2C device.

- Read register 0x10 of a device at I2C address 0x50 on I2C bus 1:
  ```sh
  sudo i2cget -y 1 0x50 0x10
  ```

#### 4. `i2cset`
`i2cset` writes data to a register of an I2C device.

- Write value 0x20 to register 0x10 of a device at I2C address 0x50 on I2C bus 1:
  ```sh
  sudo i2cset -y 1 0x50 0x10 0x20
  ```

## SPI

In Linux systems, there are many tools and packages available for debugging SPI (Serial Peripheral Interface). Here are some commonly used SPI debugging tools with detailed descriptions and usage methods:

### 1. `spidev_test`
`spidev_test` is a utility for testing SPI devices and is part of the `spidev` driver.

#### Installation
Typically, the `spidev_test` tool is located in the `tools/spi` directory of the Linux kernel source code. You can get it by compiling the kernel source code:

```sh
git clone https://github.com/torvalds/linux.git
cd linux/tools/spi
make
```

#### Usage
After compilation, an executable `spidev_test` file will be generated. Usage is as follows:

```sh
./spidev_test -D /dev/spidevX.Y
```

Parameter description:
- `-D` specifies the SPI device node, such as `/dev/spidev0.0`.
- Other parameters like `-s` set the speed, `-b` set the word length, etc.

For example, to test the `/dev/spidev0.0` device:

```sh
./spidev_test -D /dev/spidev0.0
```

## UART
In Linux systems, UART (Universal Asynchronous Receiver/Transmitter) is a common serial communication interface. Various tools and packages can be used to debug UART communication. Here are some commonly used UART debugging tools and their usage methods:

### 1. Minicom

**Package Name**: `minicom`

**Installation Method**:
```bash
sudo apt-get install minicom
```

**Usage**:
- Start `minicom`:
  ```bash
  sudo minicom -s
  ```
  The `-s` argument will start the configuration menu.
- In the configuration menu, select `Serial port setup` to configure the serial port settings. Typically, you need to set the serial device (such as `/dev/ttyUSB0`) and the baud rate (such as `115200`).
- Save the configuration and exit the setup menu. `minicom` will start monitoring the specified serial port.

### 2. Screen

**Package Name**: `screen`

**Installation Method**:
```bash
sudo apt-get install screen
```

**Usage**:
- Start `screen` for serial communication:
  ```bash
  sudo screen /dev/ttyUSB0 115200
  ```
  This command specifies the serial device `/dev/ttyUSB0` and the baud rate `115200`.
- To exit `screen`: press `Ctrl+A`, then press `K`, and finally press `Y` to confirm exit.

### 3. CuteCom

**Package Name**: `cutecom`

**Installation Method**:
```bash
sudo apt-get install cutecom
```

**Usage**:
- Start `cutecom`:
  ```bash
  sudo cutecom
  ```
- In the graphical interface, set the serial device and baud rate, then click the `Open` button to start communication.

### 4. Picocom

**Package Name**: `picocom`

**Installation Method**:
```bash
sudo apt-get install picocom
```

**Usage**:
- Start `picocom`:
  ```bash
  sudo picocom -b 115200 /dev/ttyUSB0
  ```
  In this command, `-b` specifies the baud rate, followed by the serial device.

### 5. Serial Tools

**Package Name**: `serial-tools`

**Installation Method**:
```bash
sudo apt-get install serial-tools
```

**Usage**:
- Use the `stty` command to configure the serial port parameters:
  ```bash
  sudo stty -F /dev/ttyUSB0 115200
  ```
- Use the `cat` and `echo` commands for simple serial communication:
  - Receive data:
    ```bash
    sudo cat /dev/ttyUSB0
    ```
  - Send data:
    ```bash
    sudo echo "Hello UART" > /dev/ttyUSB0
    ```

These tools and methods can help you debug and communicate with UART on Linux systems. Choose the appropriate tool according to your specific needs and configure and use them accordingly.

## CAN
In Linux environments, there are many tools available for debugging the CAN bus. Here are some common debugging tools and their usage methods:

1. **can-utils**
   - **Package Name**: can-utils
   - **Installation Method**:
     ```bash
     sudo apt-get install can-utils
     ```
   - **Common Tools**:
     - `cansend`: Sends CAN frames.
       ```bash
       cansend can0 123#1122334455667788
       ```
     - `candump`: Receives and displays CAN frames.
       ```bash
       candump can0
       ```
     - `canplayer`: Replays CAN log files.
       ```bash
       canplayer -I log-file.log
       ```
     - `cangen`: Generates random CAN frames.
       ```bash
       cangen can0
       ```

2. **socketCAN**
   - **Kernel Modules**: `can` and `can_raw`, typically included in the default Linux kernel.
   - **Configuration Method**:
     - Load the kernel modules:
       ```bash
       sudo modprobe can
       sudo modprobe can_raw
       ```
     - Configure the network interface:
       ```bash
       sudo ip link set can0 type can bitrate 500000
       sudo ip link set up can0
       ```

5. **python-can**
   - **Package Name**: python-can
   - **Installation Method**:
     ```bash
     pip install python-can
     ```
   - **Usage**:
     - Sending and receiving CAN frames.
     - Example (sending a message on the CAN bus):
       ```python
       import can

       bus = can.interface.Bus(channel='can0', bustype='socketcan')
       msg = can.Message(arbitration_id=0x123, data=[0x11, 0x22, 0x33, 0x44], is_extended_id=False)
       bus.send(msg)
       ```

These tools can help you effectively debug and analyze CAN bus communication in a Linux environment. Choose the appropriate tool and method according to your specific needs.

## Modbus

In Linux environments, there are many tools available for debugging Modbus communication. Here are some common Modbus debugging tools and their usage methods:

### 1. **mbpoll**
- **Package Name**: mbpoll
- **Installation Method**:
  ```bash
  sudo apt-get install mbpoll
  ```
- **Usage**:
  - Read holding registers:
    ```bash
    mbpoll -a 1 -r 100 -l 10 /dev/ttyUSB0
    ```
    This command reads 10 holding registers starting from address 100 from the slave device with address 1, using the `/dev/ttyUSB0` port in RTU mode.