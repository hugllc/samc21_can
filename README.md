Main Page
================

Introduction
----------------

This library is for connecting a SAMC21 to a CAN bus using the Arduino or 
PlatformIO build environments.  It is made to be compatible with MCP_CAN_lib 
(https://github.com/coryjfowler/MCP_CAN_lib) so that I can create a single library
that uses either depending on what processor is being used on the project.

Requirements
----------------

SAMC support is still not in the mainline samd (Arduino or platformio).  SAMC support
is available for Arduino here:

https://github.com/prices/ArduinoCore-samd


For platformio it is available here:

https://github.com/prices/platform-atmelsam



Using the Library
----------------

### Platformio ###

#### Installing ####
```sh
platformio platform install https://github.com/prices/platform-atmelsam
```

#### platformio.ini Example ####

```ini
[platformio]
env_default = samc21

[env:samc21]
platform = atmelsam
framework = arduino
board = samc21_xpro

lib_deps = https://github.com/hugllc/samc21_can.git

```


#### Code ####

Then for the code


```cpp
#include <samc21_can.h>

SAMC21_CAN can(0);

void setup()
{
    uint8_t ret;
    ret = can.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ);
    if (ret == CAN_OK) {
        Serial.println("CAN Initialized Successfully!");
    } else {
        Serial.println("Error Initializing CAN...");
    }
}

void loop()
{
    uint8_t ret;
    uint32_t id;
    uint8_t len;
    uint8_t buf[8];
    uint8_t i;
    
    ret = can.readMsgBuf(id, len, buffer);
    if (ret == CAN_OK) {
        Serial.print("Got a message from: ");
        Serial.print(id);
        Serial.print("  Length: ");
        Serial.print(len);
        Serial.print("  Data: ");
        for (i = 0; i < len; i++) {
            Serial.print(buf[i], HEX);
        }
        Serial.println("");
    }
}


```


License
-----------------
GPL V3


Acknowledgements
-----------------
The API was copied from https://github.com/coryjfowler/MCP_CAN_lib, as well as the
constant files mcp_can_dfs.h.  Other things might be from there, also.
