#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define STRING_VERSION_CONFIG_H __DATE__ " " __TIME__ // build date and time
#define STRING_CONFIG_H_AUTHOR "(chaky, default config)" // Who made the changes.

// Serial port 0 is still used by the Arduino bootloader regardless of this setting.
#define SERIAL_PORT 0

// This determines the communication speed of the printer
#define BAUDRATE 115200
#ifndef MOTHERBOARD
  #define MOTHERBOARD 1
#endif

#define BUFSIZE 4
#define MAX_CMD_SIZE 96





#endif //__CONFIGURATION_H
