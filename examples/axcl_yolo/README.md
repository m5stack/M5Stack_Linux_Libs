# AXCL YOLO Project

This project demonstrates YOLO (You Only Look Once) object detection using AXCL (Axera Computing Library) on Axera AX8850 devices with hardware acceleration.

## Overview

The AXCL YOLO project provides AI-powered object detection capabilities using the YOLO algorithm, leveraging the NPU (Neural Processing Unit) of Axera AX8850 devices for high-performance inference.

## Features

- **Hardware-Accelerated AI Inference**: Utilizes Axera AX8850 NPU for fast YOLO object detection
- **Multiple YOLO Model Support**: Compatible with various YOLO model formats
- **Real-time Processing**: Optimized for real-time object detection applications
- **OpenCV Integration**: Built-in image processing and visualization capabilities
- **Flexible Input Sources**: Support for various input formats and sources

## Prerequisites

- Axera AX8850 device with NPU support
- ARM64 cross-compilation toolchain (`aarch64-none-linux-gnu-`)
- AXCL MSP (Media Signal Processing) libraries
- OpenCV 4.5.5 libraries
- YOLO model files (trained models in compatible format)

## Building

1. Configure the build environment:
   ```bash
   # The project uses SCons build system
   # Ensure your toolchain is properly configured in config_defaults.mk
   ```

2. The build system will automatically clone the required axcl-samples repository:
   ```bash
   # This happens automatically during build
   git clone https://github.com/dianjixz/axcl-samples.git
   ```

3. Build the project:
   ```bash
   scons
   ```

## Architecture Support

- **ARM64 (aarch64)**: Full support with OpenCV 4.5.5 ARM64 libraries
- **x86_64**: Development support with AVX2 optimization

## Dependencies

### Core Libraries
- **AXCL Runtime**: Core AXCL runtime libraries (`axcl_rt`)
- **AXCL MSP**: Media signal processing libraries (`axcl_msp`)
- **AXCL Communication**: Device communication libraries (`axcl_comm`, `axcl_pcie_msg`, `axcl_pcie_dma`)
- **AXCL System**: System management libraries (`axcl_sys`)

### Third-Party Libraries
- **OpenCV 4.5.5**: Image processing and computer vision
  - opencv_core, opencv_highgui, opencv_imgcodecs, opencv_imgproc, opencv_videoio
- **Image Format Support**: TIFF, OpenJPEG2, JPEG-Turbo, WebP, PNG
- **Logging**: spdlog for structured logging
- **Threading**: pthread for multi-threading support

### Additional Dependencies
- **Intel TBB**: Threading Building Blocks (`libittnotify.a`)
- **Compression**: zlib for data compression
- **System Libraries**: dl (dynamic linking), tegra_hal

## Usage

The project builds multiple executables based on the source files in the axcl-samples repository. Each executable corresponds to a specific YOLO implementation or demo.

### Basic Usage Pattern
```bash
# Run a YOLO detection application
./<yolo_app_name> [options]
```

### Common Parameters
- Model file path
- Input image/video path
- Output directory
- Confidence threshold
- NMS (Non-Maximum Suppression) threshold
- Device selection

## Model Support

The project supports various YOLO model formats optimized for Axera hardware:
- YOLOv3, YOLOv4, YOLOv5, YOLOv8
- Custom trained models in compatible formats
- Models optimized for AX8850 NPU architecture

## Performance Optimization

- **NPU Acceleration**: Leverages dedicated neural processing unit
- **Memory Management**: Optimized memory allocation for real-time processing
- **Pipeline Processing**: Efficient data flow for continuous inference
- **Multi-threading**: Parallel processing for improved throughput

## Configuration

The project uses several configuration files:
- `config_defaults.mk`: Build-time configuration
- `axcl.json`: Runtime AXCL configuration (if needed)
- Model-specific configuration files

## Hardware Requirements

- **Target Platform**: Axera AX8850
- **Architecture**: ARM64 (aarch64)
- **NPU**: Required for AI inference acceleration
- **Memory**: Sufficient RAM for model loading and inference
- **Storage**: Space for model files and temporary data

## Development Notes

- The project automatically downloads the axcl-samples repository during build
- OpenCV libraries are architecture-specific (ARM64 vs x86_64)
- FFmpeg integration available for video processing workflows
- RPATH configured for proper library loading (`/usr/lib/axcl/ffmpeg`)

## Important Notes

⚠️ **OpenCV Video Library Limitation**: The OpenCV library included with the AXCL MSP SDK does not support video processing functionalities. If you want to use video-related examples such as `ax_yolo11_steps_video`, you need to execute the following command in the build terminal:
```bash
export CONFIG_USE_HOST_OPENCV4LIB=y
```
This will enable the use of the host's OpenCV library with video support during compilation.

To enable video functionality, please follow these steps:
1. Set the environment variable `CONFIG_USE_HOST_OPENCV4LIB=y`
2. Install `libopencv4-dev` on your host machine
3. Ensure that the host OpenCV library is compatible with your target architecture

## Troubleshooting

1. **Build Issues**:
   - Ensure proper toolchain configuration
   - Verify OpenCV library paths for your architecture
   - Check network connectivity for repository cloning

2. **Runtime Issues**:
   - Verify AXCL libraries are properly installed
   - Ensure model files are in correct format and location
   - Check device permissions and NPU availability

3. **Performance Issues**:
   - Monitor NPU utilization
   - Optimize input resolution for your use case
   - Adjust batch size and threading parameters

## Examples

The axcl-samples repository contains various example implementations:
- Basic YOLO object detection
- Real-time video processing
- Batch image processing
- Custom model integration


## External Dependencies

- axcl-samples: https://github.com/dianjixz/axcl-samples.git
- OpenCV 4.5.5: Computer vision library
- Various image format libraries (TIFF, JPEG, PNG, WebP)
