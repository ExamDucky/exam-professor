# Student Exam USB Device

## Overview
This USB device is designed to automate and monitor the exam-taking process. Based on the ESP32-S3 microcontroller, it facilitates secure exam distribution, process monitoring, and submission while ensuring academic integrity.

## Features
- Automatic connection to exam server access point
- Secure exam file download and transmission
- Real-time process monitoring
- Automated screenshot capture
- Exam submission handling
- File system management using FFat

## Hardware Requirements
- ESP32-S3 microcontroller
- USB connection capability
- Sufficient storage for exam files and monitoring data

## Software Components

### Firmware (Arduino Framework)
- `esp_exam_slave.ino`: Main device firmware
- `serial_parser.ino`: Serial communication handler
- `student_http.ino`: HTTP client implementation

### System Service (Python)
- `serial_client.py`: Serial communication client
- `process_handler.py`: System monitoring and screenshot capture

## Setup Instructions

1. Flash the ESP32-S3 firmware:
   ```bash
   # Using Arduino IDE or PlatformIO
   # Select ESP32-S3 board
   # Upload the firmware files
   ```

2. Install Python dependencies:
   ```bash
   pip install pyserial psutil mss pillow
   ```

3. Configure the device:
   - Update WiFi credentials in `esp_exam_slave.ino`:
     ```cpp
     const char* ssid = "test-ssid";
     const char* password = "test-password";
     ```
   - Set student information:
     ```cpp
     String studentName = "Your Name";
     String studentInfo = "Your ID";
     ```

## Usage

### Device Operation
1. Connect the device to a USB port
2. Device automatically:
   - Connects to the exam server
   - Registers student information
   - Starts monitoring processes
   - Waits for exam availability

### During Exam
The device will:
- Download exam files when available
- Monitor active processes every 60 seconds
- Take periodic screenshots
- Store all monitoring data locally

### Exam Submission
1. When ready to submit:
   ```python
   python serial_client.py submit path/to/exam/solution
   ```
2. Device will:
   - Upload the solution
   - Send final monitoring data
   - Confirm submission with server

## File Structure
```
/
├── firmware/
│   ├── esp_exam_slave.ino
│   ├── serial_parser.ino
│   └── student_http.ino
├── service/
│   ├── serial_client.py
│   └── process_handler.py
└── FFat/
    ├── exam.zip
    ├── processes_[ID].txt
    └── scrt_[ID].png
```

## Communication Protocol

### Serial Commands
- `PSTART/PEND`: Process list transfer
- `SCSTART/SCEND`: Screenshot transfer
- `EXSUB/EXFIN`: Exam submission
- `PROCGET`: Request process list
- `EXDOW/EXEND`: Exam download

### HTTP Endpoints
- `/login`: Student registration
- `/processes`: Process list upload
- `/upload`: File upload (screenshots/exam)
- `/example_file`: file download
- `/submit`: Submission confirmation
- `/exists`: Check exam availability

## Security Features
- Unique student identification
- MAC address registration
- Secure file transfer
- Process monitoring
- Screenshot verification
