# Dynamic Real-Time Image Processing Pipeline Using FreeRTOS

A real-time image processing pipeline demonstrating FreeRTOS task scheduling, dynamic priority adaptation, task notifications, timing analysis, and deadline-aware execution.

## Overview

This project implements a real-time image processing pipeline using **FreeRTOS and OpenCV** in a host-based environment.

The system separates image processing into independent FreeRTOS tasks and dynamically adjusts task priorities according to simulated execution delays. The main objective is to maintain a **100 ms pipeline timing budget** despite unpredictable delays.

The project demonstrates how real-time scheduling strategies can be applied to embedded systems where execution-time variability can affect deadline compliance.

## System Architecture

The pipeline consists of five main tasks:

- **Capture** — captures and crops the input image
- **Grayscale** — converts the image to grayscale
- **Blur** — applies image blurring
- **Edge Detection** — performs edge detection
- **Invert** — applies image inversion

Task synchronization is implemented using **FreeRTOS task notifications**, while task priorities control the execution order of the processing stages.

## Real-Time Scheduling

The system uses FreeRTOS preemptive scheduling with configurable task priorities.

A random delay between **0 and 90 ms** is introduced into the Invert task to simulate unpredictable disturbances such as I/O stalls or computational overload.

The system knows the delay before task execution and dynamically adjusts the Invert task's priority:

| Invert Delay | Priority Adaptation |
|---|---|
| ≤ 25 ms | No promotion |
| > 25 ms | Increase by one level |
| > 35 ms | Increase further |
| > 50 ms | Highest processing priority |

This allows the scheduling strategy to adapt to expected timing disturbances.

## Timing Analysis

Each processing task measures its execution time using the FreeRTOS tick counter.

The system calculates the total pipeline execution time and compares it against the **100 ms real-time budget**.

The program reports whether the pipeline remains within the required deadline.

## Key Concepts Demonstrated

- FreeRTOS task creation and scheduling
- Preemptive priority-based scheduling
- Task notifications
- Task synchronization
- Dynamic priority adaptation
- Blocking-aware scheduling
- Execution-time measurement
- Real-time deadline monitoring
- Image processing with OpenCV
- Embedded systems design principles

## Project Structure

```text
freertos-realtime-image-processing/
│
├── main.c
├── FreeRTOSConfig.h
└── Dynamic Real-Time Image Processing Pipeline Using FreeRTOS.pdf


Technologies
C
FreeRTOS
OpenCV
Real-Time Operating Systems
Task Scheduling
Embedded Systems
Image Processing
Execution Environment

The project runs on a host machine to simulate embedded real-time behavior. Although it is not deployed on physical embedded hardware, the task architecture and scheduling logic follow real-time embedded-system principles.

Results

The project demonstrates that scheduling decisions can be adapted dynamically when execution-time disturbances are known in advance.
