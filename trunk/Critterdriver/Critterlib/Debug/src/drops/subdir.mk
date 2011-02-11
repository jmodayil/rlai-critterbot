################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/drops/CritterControlDrop.cpp \
../src/drops/CritterStateDrop.cpp \
../src/drops/USeconds.cpp 

OBJS += \
./src/drops/CritterControlDrop.o \
./src/drops/CritterStateDrop.o \
./src/drops/USeconds.o 

CPP_DEPS += \
./src/drops/CritterControlDrop.d \
./src/drops/CritterStateDrop.d \
./src/drops/USeconds.d 


# Each subdirectory must supply rules for building sources it contributes
src/drops/%.o: ../src/drops/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -m32 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


