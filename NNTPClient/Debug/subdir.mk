################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Comando.cpp \
../NNTPClientBO.cpp \
../NNTPClientDAO.cpp \
../ParseadorValidadorDeComandos.cpp \
../nntpClient.cpp 

OBJS += \
./Comando.o \
./NNTPClientBO.o \
./NNTPClientDAO.o \
./ParseadorValidadorDeComandos.o \
./nntpClient.o 

CPP_DEPS += \
./Comando.d \
./NNTPClientBO.d \
./NNTPClientDAO.d \
./ParseadorValidadorDeComandos.d \
./nntpClient.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


