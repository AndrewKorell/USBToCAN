# USB To CAN 

This project will communicate through USB via the a terminal program like puTTY connected through a VCP created by USB OTG (On the Go).

The USB port is configured as a Client device using Full Speed (12MB/s). HS is not available as it would require a separate PHY. 

I am implementing this with an STM32F446ZE.

This is project also utilizes CMSIS2 middleware implementation of FreeRTOS. 

# CMD Line Interface 


| Command        | Input                                         | Response                                       |
|----------------|-----------------------------------------------|----------------------------------------------------------|
| Read CO Value  | READ <nodeid> <index> <sub-index>             | READ <nodeid> <index> <sub-index> <val> <status> <ticks> |
| Write CO Value | READ <nodeid> <index> <sub-index> <val>       | READ <nodeid> <index> <sub-index> <val> <status> <ticks> |
| Start CO Sync  | SYNC START <time_ms>                          | SYNC START <time_ms> <status> <ticks>                    |
| STOP CO Sync   | SYNC STOP                                     | SYNC STOP <status> <ticks>                               |
| HELP           | HELP                                          | Print help text                                          |
| RTC MENU       | RTC                                           | Real Time Clock Config Guide                             | 
| BAUD           | BAUD <rate_bits_per_s>                        | BAUD <rate_bits_per_s> <status> <ticks>                  |       
| STATUS         | STATUS                                        | DONGLE <dongle_status> CAN <can_status>                  | 
