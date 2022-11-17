################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/EventQueue.cpp \
../src/JsonParser.cpp \
../src/Main.cpp \
../src/ReqProcessor.cpp 

CPP_DEPS += \
./src/EventQueue.d \
./src/JsonParser.d \
./src/Main.d \
./src/ReqProcessor.d 

OBJS += \
./src/EventQueue.o \
./src/JsonParser.o \
./src/Main.o \
./src/ReqProcessor.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/EventQueue.d ./src/EventQueue.o ./src/JsonParser.d ./src/JsonParser.o ./src/Main.d ./src/Main.o ./src/ReqProcessor.d ./src/ReqProcessor.o

.PHONY: clean-src

