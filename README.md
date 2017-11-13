Main Page                            {#mainpage}
================

Introduction
----------------

This library is for connecting the HUGnetCAN devices together in a network.

Requirements
----------------
These will be install automatically when using platformio, but will have to be installed
manually when using the Arduino IDE.

 - [MCP_CAN_lib](https://github.com/coryjfowler/MCP_CAN_lib.git) by Cory Fowler 
    at https://github.com/coryjfowler/MCP_CAN_lib.git
   - ZIP archive: https://github.com/coryjfowler/MCP_CAN_lib/archive/master.zip
 - Time Library (ONE of the following):
   - [Time](https://github.com/PaulStoffregen/Time.git) by Paul Stoffregen at https://github.com/PaulStoffregen/Time.git
     - ZIP archive: https://github.com/PaulStoffregen/Time/archive/master.zip
   - Time libary in the Arduino Library Manager by Michael Margolis

Using the Library
----------------

Using the default pins for chip select and interrupt
```cpp
#include <HUGnetCANLib.h>

// Create a HUGnetCAN object
HUGnetCAN dev;

```

Using custom pins for chip select and interrupt
```cpp
#include <HUGnetCANLib.h>

// Create a HUGnetCAN object
HUGnetCAN dev(
    CS_PIN,          // The Chip select pin goes here
    INT_PIN          // The interrupt pin goes here
);

```

See [Receiving](@ref receiving_frames) and [Sending](@ref sending_frames) for more information.

License
-----------------
Proprietary to Hunt Utilities Group, LLC


Contact
-----------------
```
Hunt Utilities Group, LLC
2331 Dancing Wind Rd SW
Pine River, MN 56474
(218) 587-5001
http://www.hugllc.com/
```