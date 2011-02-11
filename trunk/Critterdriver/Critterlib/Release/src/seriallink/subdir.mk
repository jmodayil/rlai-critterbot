################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/seriallink/ActionWriter.cpp \
../src/seriallink/CritterSerialLink.cpp 

OBJS += \
./src/seriallink/ActionWriter.o \
./src/seriallink/CritterSerialLink.o 

CPP_DEPS += \
./src/seriallink/ActionWriter.d \
./src/seriallink/CritterSerialLink.d 


# Each subdirectory must supply rules for building sources it contributes
src/seriallink/%.o: ../src/seriallink/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


