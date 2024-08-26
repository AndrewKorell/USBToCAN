# USB To CAN 

This project will communicate through USB via the a terminal program like puTTY connected through a VCP created by USB OTG (On the Go).

The USB port is configured as a Client device using Full Speed (12MB/s). HS is not available as it would require a separate PHY. 

I am implementing this with an STM32F446ZE.

This is project also utilizes CMSIS2 middleware implementation of FreeRTOS. 

# CMD Line Interface 


| Command        | Input                                    | Response                                             |
|----------------|------------------------------------------|------------------------------------------------------|
| Read CO Value  | READ $nodeid $index $sub-index           | READ $nodeid $index> $sub-index $val $status $ticks> |
| Write CO Value | WRITE $nodeid $index $sub-index val      | READ $nodeid $index> $sub-index $val $status $ticks> |
| Start CO Sync  | SYNC START $time_ms                      | SYNC START $time_ms $status $ticks                   |
| STOP CO Sync   | SYNC STOP                                | SYNC STOP $status $ticks                             |
| HELP           | HELP                                     | Print help text                                      |
| RTC MENU       | RTC                                      | Real Time Clock Config Guide                         | 
| BAUD           | BAUD $rate_bits_per_s                    | BAUD $rate_bits_per_s $status $ticks                 |       
| STATUS         | STATUS                                   | DONGLE $dongle_status CAN $can_status                | 

## Swimlane for SDO and Menu

Chart doesn't show how NMT or SYNC will be handled but should show the asynchronour relationship between thhe input and output.

The queues listed here are standard FreeRTOS queues. I have not shown the WAIT/NOTIFY relationships between the processes.

READ REQUEST
1. Processed, if request is malform send to response queue
2. Generate SDO and send to mailbox 1
3. If send to mailbox 1, if HAL returns error send to response queue
4. If send to mailbox successful add request to wait queue

PROCESS WAIT
1. check for timeout on items in queue, if item timed out send to response queue 

PROCESS CAN RX
1. Wait for response on RX Interrupt
2. RX interrupt occurs, check if we're waiting for the response
3. Check for error and timeout
4. Send to response queue 

![USB_CANOpen_Dongle drawio](https://github.com/user-attachments/assets/a3e59fbd-9adc-4a7b-ad1c-56517a1c16a4)
