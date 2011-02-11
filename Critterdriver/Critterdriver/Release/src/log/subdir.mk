################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/log/LatencyLogger.cpp 

OBJS += \
./src/log/LatencyLogger.o 

CPP_DEPS += \
./src/log/LatencyLogger.d 


# Each subdirectory must supply rules for building sources it contributes
src/log/%.o: ../src/log/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/degris/workspace/Critterdriver/Critterlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


