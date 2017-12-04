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

#### platformio.ini ####
The platformio.ini file should look like this:

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




```


License
-----------------
GPL V3


Acknowledgements
-----------------
The API was copied from https://github.com/coryjfowler/MCP_CAN_lib, as well as the
constant files mcp_can_dfs.h.  Other things might be from there, also.
