# AXCL Sample Project

This project contains sample applications demonstrating the usage of AXCL (Axera Computing Library) for video processing on Axera AX8850 devices.

## Overview

The AXCL Sample project provides three main sample applications:

1. **System Information Tool** (`sys`) - Device information and monitoring
2. **Video Decoder** (`vdec`) - Video decoding functionality
3. **Video Encoder** (`venc`) - Video encoding functionality

## Features

- **Device Management**: Initialize and manage Axera AX8850 devices
- **Video Decoding**: Decode H.264/H.265 video streams with hardware acceleration
- **Video Encoding**: Encode video streams using hardware video encoders
- **System Monitoring**: Monitor device status, memory usage, CPU utilization, NPU usage, and thermal information

## Prerequisites

- Axera AX8850 device
- ARM64 cross-compilation toolchain (`aarch64-none-linux-gnu-`)
- AXCL MSP (Media Signal Processing) libraries
- FFmpeg libraries for video demuxing

## Building

1. Configure the build environment:
   ```bash
   # The project uses SCons build system
   # Ensure your toolchain is properly configured in config_defaults.mk
   ```

2. Build the project:
   ```bash
   scons
   ```

## Sample Applications

### 1. System Information Tool (`sys`)

Displays comprehensive device information including:
- Memory usage (RAM and CMM)
- CPU utilization
- NPU (Neural Processing Unit) utilization
- Device temperature
- Fan speed

**Usage:**
```bash
./sys
```

### 2. Video Decoder (`vdec`)

Decodes video files (MP4, H.264, H.265) using hardware acceleration.

**Usage:**
```bash
./vdec -i <input_file> [options]
```

**Options:**
- `-i, --url`: Input video file path (required)
- `-d, --device`: Device index (default: 0)
- `--count`: Number of decoder groups (default: 1)
- `--json`: AXCL configuration file path (default: ./axcl.json)
- `-w, --width`: Frame width (default: 1920)
- `-h, --height`: Frame height (default: 1080)
- `--VdChn`: Channel ID (default: 0)
- `--yuv`: Transfer NV12 from device (default: 0)
- `-o, --output`: Output file path

**Example:**
```bash
./vdec -i sample.mp4 -o output.yuv --yuv 1
```

### 3. Video Encoder (`venc`)

Encodes video streams using hardware video encoders.

**Usage:**
```bash
./venc [options]
```

Refer to the help documentation (`help.dat`) for detailed parameter information.

## Configuration

The project uses configuration files:
- `config_defaults.mk`: Build configuration
- `axcl.json`: Runtime AXCL configuration (if needed)

## Dependencies

- **AXCL Libraries**: Core AXCL runtime and MSP libraries
- **FFmpeg**: For video demuxing and format support
- **OpenCV**: For image processing (in some components)
- **pthread**: For multi-threading support

## Hardware Support

- **Target Platform**: Axera AX8850
- **Architecture**: ARM64 (aarch64)
- **Hardware Acceleration**: Video encoding/decoding, NPU processing

## Notes

- Ensure proper device permissions for accessing Axera hardware
- The applications require root privileges or proper udev rules for device access
- Video formats supported depend on the hardware decoder capabilities
- Output formats are typically NV12 for decoded video and compressed streams for encoded video

## Troubleshooting

1. **Device not found**: Ensure the Axera device is properly connected and recognized
2. **Permission denied**: Run with appropriate privileges or configure udev rules
3. **Library not found**: Verify AXCL libraries are properly installed and in the library path
4. **Decoding errors**: Check input file format compatibility and device capabilities

