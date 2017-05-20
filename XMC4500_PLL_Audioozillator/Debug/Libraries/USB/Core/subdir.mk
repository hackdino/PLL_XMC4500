################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/USB/Core/ConfigDescriptors.c \
../Libraries/USB/Core/DeviceStandardReq.c \
../Libraries/USB/Core/Events.c \
../Libraries/USB/Core/HostStandardReq.c \
../Libraries/USB/Core/USBTask.c 

OBJS += \
./Libraries/USB/Core/ConfigDescriptors.o \
./Libraries/USB/Core/DeviceStandardReq.o \
./Libraries/USB/Core/Events.o \
./Libraries/USB/Core/HostStandardReq.o \
./Libraries/USB/Core/USBTask.o 

C_DEPS += \
./Libraries/USB/Core/ConfigDescriptors.d \
./Libraries/USB/Core/DeviceStandardReq.d \
./Libraries/USB/Core/Events.d \
./Libraries/USB/Core/HostStandardReq.d \
./Libraries/USB/Core/USBTask.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/USB/Core/%.o: ../Libraries/USB/Core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DXMC4500_F100x1024 -I"/home/hackdino/Documents/mbed_master_thesis/Workspace_Test_XMCLib/XMC4500_Blinky/Libraries/CMSIS/Infineon/XMC4500_series/Include" -I"/home/hackdino/Documents/mbed_master_thesis/Workspace_Test_XMCLib/XMC4500_Blinky/Libraries/USB" -I"/home/hackdino/Documents/mbed_master_thesis/Workspace_Test_XMCLib/XMC4500_Blinky/Libraries/USB/GLUE" -I"/home/hackdino/Documents/mbed_master_thesis/Workspace_Test_XMCLib/XMC4500_Blinky/Libraries/USBH_VCOM" -I"/home/hackdino/Documents/mbed_master_thesis/Workspace_Test_XMCLib/XMC4500_Blinky/Libraries/CMSIS/Include" -I"/home/hackdino/Documents/mbed_master_thesis/Workspace_Test_XMCLib/XMC4500_Blinky/Libraries/XMCLib/inc" -I"/home/hackdino/Documents/mbed_master_thesis/Workspace_Test_XMCLib/XMC4500_Blinky/Libraries" -I"/home/hackdino/Documents/mbed_master_thesis/Workspace_Test_XMCLib/XMC4500_Blinky" -std=gnu99 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


