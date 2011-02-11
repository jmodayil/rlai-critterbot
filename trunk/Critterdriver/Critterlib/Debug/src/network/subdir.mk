################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/network/Server.cpp \
../src/network/SyncActionBuffer.cpp 

OBJS += \
./src/network/Server.o \
./src/network/SyncActionBuffer.o 

CPP_DEPS += \
./src/network/Server.d \
./src/network/SyncActionBuffer.d 


# Each subdirectory must supply rules for building sources it contributes
src/network/%.o: ../src/network/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -m32 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


