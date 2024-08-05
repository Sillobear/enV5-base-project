

In this file the communication protocol is explained.

# Communication Protocol

The communication is driven by the external device to determine the elasticNodes behavior.
The communication protocol is as follows:




## Request to enV5
| 1 bytes | 4 bytes      | payload-size | Checksum-size |
|---------|--------------|--------------|---------------|
| command | payload-size | data         | checksum      |

## Answer from enV5
| 1 bytes | 4 bytes      | payload-size | Checksum-size |
|---------|--------------|--------------|---------------|
| command | payload-size | data         | checksum      |

## Checksum-function 
1 byte XNOR over all command + payload-size + data bytes

## Commands
The first 128 are our commands.
The second 128 can be user specific commands.

| command                   | uint8 |
|---------------------------|-------|
| nack                      | 0     |
| ack                       | 1     |
| device-id                 | 2     |
| send data to ram          | 3     |
| read data from ram        | 4     |
| send data to flash        | 5     |
| read data from flash      | 6     |
| inference with ram-data   | 7     |
| inference with flash-data | 8     |
| training with ram-data    | 9     |
| training with flash-data  | 10    |

