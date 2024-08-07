# USB to CAN Communication Project

Welcome to the USB to CAN Communication Project. This document provides an overview of the project's codebase, development tools, and instructions on how to set up and use the project.

# Project Overview

The USB to CAN project is designed to interface a USB device with a CAN bus, facilitating communication between the two. The project uses an STM32 microcontroller and relies on the STM32CubeMX tool for peripheral configuration and HAL library code generation.

# Code Architecture

The project is structured into multiple modules, each with a specific role:

- **Device Abstraction Layer**: Provides a hardware abstraction layer for various peripherals like LEDs, buzzers, keys, CAN, and USB-CDC. This layer includes:
  - `device_led.h/c`: LED device control.
  - `device_buzzer.h/c`: Buzzer device control.
  - `device_key.h/c`: Key input handling.
  - `device_usb2can.h/c`: USB to CAN communication specific functionalities.

- **Task Scheduling**: Manages task execution using a simple scheduler:
  - `task_schedule.h/c`: Task scheduling and management.
  - `task_led.h/c`: LED blinking task.
  - `task_buzzer.h/c`: Buzzer task for sound output.
  - `task_key.h/c`: Key handling task.
  - `task_usb2can.h/c`: Main task for USB to CAN communication.

- **Utility Functions**: Miscellaneous utility functions and defines.

# Development Tools

- **Keil MDK-ARM**: The Integrated Development Environment (IDE) used for coding, building, and debugging the project.
- **Keil AC6**: The ARM Cortex-M compiler used for compiling the project, known for its performance optimizations.
- **Keil Build Viewer**: An open-source tool for enhancing the display of Keil build information. [GitHub](https://gitee.com/DinoHaw/keil-build-viewer)
- **J-Link Debugger**: Used for debugging and flashing the STM32 microcontroller.
- **SEGGER RTT**: Used in conjunction with J-Link for real-time logging.

# Setup and Usage

1. **Environment Setup**: Install Keil MDK-ARM and the necessary ARM Cortex-M compiler (AC6).
2. **Project Import**: Import the project files into your Keil MDK-ARM IDE.
3. **Configuration**:
   - Include the header files as needed.
   - Modify macros and enums in the respective header files.
   - Adjust resources in the source files to match your hardware setup.
4. **Device Registration**: Use the provided functions to register devices and their callbacks.
5. **Task Initialization**: Initialize tasks using the `schedule_register` function.
6. **Execution**: Start the scheduler by calling `schedule_start` in your main loop.

# debug problem record

