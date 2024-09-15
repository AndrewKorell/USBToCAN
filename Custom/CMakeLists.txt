
add_compile_definitions(STM32F446xx)

include_directories(../Drivers/STM32F4xx_HAL_Driver/Inc/)

set(CAN_OPEN_SRC can_open.c can_open.h)
project(can_open C)
add_library(can_open ${CAN_OPEN_SRC})