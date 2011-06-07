################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../critterbot_power.c \
../power_avr_charge.c \
../power_avr_utils.c 

OBJS += \
./critterbot_power.o \
./power_avr_charge.o \
./power_avr_utils.o 

C_DEPS += \
./critterbot_power.d \
./power_avr_charge.d \
./power_avr_utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/usr/local/CrossPack-AVR-20090415/avr-4/include -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega324p -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


