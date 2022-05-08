# ESP32-uC_EdgeProcessingBridge
ESP32 based hardware serial bridge for adding Edge Embedded processing capabilities to existing low-powered microcontrollers.

TCP to UART bridge in AP WiFi mode.

Compile ESP32_Serial_Bridge in the Arduino IDE and upload to ESP32

Works in combination with a PIC uC device connected through a serial port to UART1 of the ESP32

PIC example code provided is for the INA260 Current/Voltage/Power monitoring IC, data output is formatted to easily be input to Edge Impulse

Data output should be copy/pasted into an Excel Document, formatted into cells via the commas separating each data value, and saved as a .csv file to be uploadable to Edge Impulse

IP Address of ESP32: 192.168.4.1

SSID: Network Name Here (default in code, change to desired)

Password: Password Here (default in code, change to desired)

Port 8881 utilized for COM1

Install ESP32 arduino libraries found at:

https://github.com/espressif/arduino-esp32

///////////////////////////////////////////

Pin Connectivity:

COM1 Rx <-> GPIO16

COM1 Tx <-> GPIO17

///////////////////////////////////////////

# PIC_Driver
PIC utilized was a PIC18F27q43, but your selection will vary and required MCC files must be generated for your specific application

Utilized MPLAB X IDE for PIC programming

Setup UART1 pins in MCC

Example formatting for data output can be found in PIC_Driver case 'x'

Menu Readout included is not necessary, any data can be formatted and continuously output to UART1
