################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Crittermeasure.cpp \
../src/MeasuredActionWriter.cpp \
../src/MeasuredSerialLink.cpp \
../src/Measurements.cpp \
../src/MocActionBuffer.cpp 

OBJS += \
./src/Crittermeasure.o \
./src/MeasuredActionWriter.o \
./src/MeasuredSerialLink.o \
./src/Measurements.o \
./src/MocActionBuffer.o 

CPP_DEPS += \
./src/Crittermeasure.d \
./src/MeasuredActionWriter.d \
./src/MeasuredSerialLink.d \
./src/Measurements.d \
./src/MocActionBuffer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/degris/workspace/Critterdriver/Critterlib/src" -O0 -g3 -Wall -c -fmessage-length=0 -m32 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


