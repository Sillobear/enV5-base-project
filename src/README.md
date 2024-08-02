

In this file the communication protocol is explained.

# Communication Protocol

The communication is driven by the external device to determine the elasticNodes behavior.
The communication protocol is as follows:





| 0            | 1-7      |
|--------------|----------|
| start/finish | command  |

| command                   | uint7 |
|---------------------------|-------|
| device-id                 | 0     |
| send data to ram          | 11    |
| read data from ram        | 12    |
| send data to flash        | 20    |
| read data from flash      | 21    |
| inference with ram-data   | 30    |
| inference with flash-data | 31    |
| training with ram-data    | 50    |
| training with flash-data  | 51    |

### send data to ram
Here is the communication flow for sending data to flash
1. Device: start bit + command (7 bits) + data-length(4bytes integer)
2. Env5: expected slice length (4 bytes integer)
3. Repeat as often as wanted
    1. Env5: start bit + command (7 bits) +  slice-id (4 bytes integer)
    2. Device: start bit + command (7bits) + Send data slice(expected slice length bytes)
4. Env5: command(7 bits) + finish bit + data-checksum ()
5. Device: command(7 bits) + finish bit + data-checksum ()

### send data to flash
Here is the communication flow for sending data to flash
1. Device: start bit + command (7 bits) + flash address (4 bytes) + data-length(4bytes integer)
2. Env5: expected slice length (4 bytes integer)
3. Repeat as often as wanted
   1. Env5: start bit + command (7 bits) +  slice-id (4 bytes integer)
   2. Device: Send data slice(expected slice length bytes)
4. Env5: command(7 bits) + finish bit + data-checksum ()
5. Device: command(7 bits) + finish bit + data-checksum ()




### inference
1. Device: start bit + command (7 bits) + data-length(4 bytes)
2. Env5: expected data-length 
3. 