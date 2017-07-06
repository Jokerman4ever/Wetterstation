################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../src/ \
../src/ASF/ \
../src/ASF/common/ \
../src/ASF/common/boards/ \
../src/ASF/common/boards/user_board/ \
../src/ASF/common/services/ \
../src/ASF/common/services/clock/ \
../src/ASF/common/services/clock/xmega/ \
../src/ASF/common/services/sleepmgr/ \
../src/ASF/common/services/sleepmgr/xmega/ \
../src/ASF/common/utils/ \
../src/ASF/common/utils/interrupt/ \
../src/ASF/common/utils/make/ \
../src/ASF/xmega/ \
../src/ASF/xmega/drivers/ \
../src/ASF/xmega/drivers/cpu/ \
../src/ASF/xmega/drivers/rtc/ \
../src/ASF/xmega/drivers/sleep/ \
../src/ASF/xmega/utils/ \
../src/ASF/xmega/utils/assembler/ \
../src/ASF/xmega/utils/bit_handling/ \
../src/ASF/xmega/utils/preprocessor/ \
../src/config/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../ADC/adc.c \
../BH1750.c \
../com.c \
../i2c.c \
../main.c \
../src/ASF/common/boards/user_board/init.c \
../src/ASF/common/services/clock/xmega/sysclk.c \
../src/ASF/common/services/sleepmgr/xmega/sleepmgr.c \
../src/ASF/xmega/drivers/rtc/rtc.c


PREPROCESSING_SRCS +=  \
../src/ASF/xmega/drivers/cpu/ccp.s


ASM_SRCS += 


OBJS +=  \
adc.o \
BH1750.o \
com.o \
i2c.o \
main.o \
src/ASF/common/boards/user_board/init.o \
src/ASF/common/services/clock/xmega/sysclk.o \
src/ASF/common/services/sleepmgr/xmega/sleepmgr.o \
src/ASF/xmega/drivers/cpu/ccp.o \
src/ASF/xmega/drivers/rtc/rtc.o

OBJS_AS_ARGS +=  \
adc.o \
BH1750.o \
com.o \
i2c.o \
main.o \
src/ASF/common/boards/user_board/init.o \
src/ASF/common/services/clock/xmega/sysclk.o \
src/ASF/common/services/sleepmgr/xmega/sleepmgr.o \
src/ASF/xmega/drivers/cpu/ccp.o \
src/ASF/xmega/drivers/rtc/rtc.o

C_DEPS +=  \
adc.d \
BH1750.d \
com.d \
i2c.d \
main.d \
src/ASF/common/boards/user_board/init.d \
src/ASF/common/services/clock/xmega/sysclk.d \
src/ASF/common/services/sleepmgr/xmega/sleepmgr.d \
src/ASF/xmega/drivers/cpu/ccp.d \
src/ASF/xmega/drivers/rtc/rtc.d

C_DEPS_AS_ARGS +=  \
adc.d \
BH1750.d \
com.d \
i2c.d \
main.d \
src/ASF/common/boards/user_board/init.d \
src/ASF/common/services/clock/xmega/sysclk.d \
src/ASF/common/services/sleepmgr/xmega/sleepmgr.d \
src/ASF/xmega/drivers/cpu/ccp.d \
src/ASF/xmega/drivers/rtc/rtc.d

OUTPUT_FILE_PATH +=WRL_Sensorknoten.elf

OUTPUT_FILE_PATH_AS_ARGS +=WRL_Sensorknoten.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler



















./%.o: ../ADC/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -DDEBUG -DF_CPU=2000000UL -DBOARD=USER_BOARD  -I"C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\include" -I"../src/ASF/common/boards/user_board" -I"../src/ASF/common/boards" -I"../src/ASF/xmega/utils/preprocessor" -I"../src/ASF/xmega/utils" -I"../src/ASF/common/utils" -I"../src" -I"../src/config" -I"../src/ASF/xmega/drivers/cpu" -I"../src/ASF/xmega/drivers/rtc" -I"../src/ASF/xmega/drivers/sleep" -I"../src/ASF/common/services/clock" -I"../src/ASF/common/services/sleepmgr"  -O0 -fdata-sections -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g3 -Wall -mmcu=atxmega32d4 -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\gcc\dev\atxmega32d4" -c -std=gnu99 -fno-strict-aliasing -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -mrelax -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -DDEBUG -DF_CPU=2000000UL -DBOARD=USER_BOARD  -I"C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\include" -I"../src/ASF/common/boards/user_board" -I"../src/ASF/common/boards" -I"../src/ASF/xmega/utils/preprocessor" -I"../src/ASF/xmega/utils" -I"../src/ASF/common/utils" -I"../src" -I"../src/config" -I"../src/ASF/xmega/drivers/cpu" -I"../src/ASF/xmega/drivers/rtc" -I"../src/ASF/xmega/drivers/sleep" -I"../src/ASF/common/services/clock" -I"../src/ASF/common/services/sleepmgr"  -O0 -fdata-sections -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g3 -Wall -mmcu=atxmega32d4 -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\gcc\dev\atxmega32d4" -c -std=gnu99 -fno-strict-aliasing -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -mrelax -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/boards/user_board/%.o: ../src/ASF/common/boards/user_board/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -DDEBUG -DF_CPU=2000000UL -DBOARD=USER_BOARD  -I"C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\include" -I"../src/ASF/common/boards/user_board" -I"../src/ASF/common/boards" -I"../src/ASF/xmega/utils/preprocessor" -I"../src/ASF/xmega/utils" -I"../src/ASF/common/utils" -I"../src" -I"../src/config" -I"../src/ASF/xmega/drivers/cpu" -I"../src/ASF/xmega/drivers/rtc" -I"../src/ASF/xmega/drivers/sleep" -I"../src/ASF/common/services/clock" -I"../src/ASF/common/services/sleepmgr"  -O0 -fdata-sections -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g3 -Wall -mmcu=atxmega32d4 -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\gcc\dev\atxmega32d4" -c -std=gnu99 -fno-strict-aliasing -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -mrelax -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/services/clock/xmega/%.o: ../src/ASF/common/services/clock/xmega/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -DDEBUG -DF_CPU=2000000UL -DBOARD=USER_BOARD  -I"C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\include" -I"../src/ASF/common/boards/user_board" -I"../src/ASF/common/boards" -I"../src/ASF/xmega/utils/preprocessor" -I"../src/ASF/xmega/utils" -I"../src/ASF/common/utils" -I"../src" -I"../src/config" -I"../src/ASF/xmega/drivers/cpu" -I"../src/ASF/xmega/drivers/rtc" -I"../src/ASF/xmega/drivers/sleep" -I"../src/ASF/common/services/clock" -I"../src/ASF/common/services/sleepmgr"  -O0 -fdata-sections -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g3 -Wall -mmcu=atxmega32d4 -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\gcc\dev\atxmega32d4" -c -std=gnu99 -fno-strict-aliasing -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -mrelax -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/services/sleepmgr/xmega/%.o: ../src/ASF/common/services/sleepmgr/xmega/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -DDEBUG -DF_CPU=2000000UL -DBOARD=USER_BOARD  -I"C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\include" -I"../src/ASF/common/boards/user_board" -I"../src/ASF/common/boards" -I"../src/ASF/xmega/utils/preprocessor" -I"../src/ASF/xmega/utils" -I"../src/ASF/common/utils" -I"../src" -I"../src/config" -I"../src/ASF/xmega/drivers/cpu" -I"../src/ASF/xmega/drivers/rtc" -I"../src/ASF/xmega/drivers/sleep" -I"../src/ASF/common/services/clock" -I"../src/ASF/common/services/sleepmgr"  -O0 -fdata-sections -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g3 -Wall -mmcu=atxmega32d4 -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\gcc\dev\atxmega32d4" -c -std=gnu99 -fno-strict-aliasing -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -mrelax -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/xmega/drivers/rtc/%.o: ../src/ASF/xmega/drivers/rtc/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -DDEBUG -DF_CPU=2000000UL -DBOARD=USER_BOARD  -I"C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\include" -I"../src/ASF/common/boards/user_board" -I"../src/ASF/common/boards" -I"../src/ASF/xmega/utils/preprocessor" -I"../src/ASF/xmega/utils" -I"../src/ASF/common/utils" -I"../src" -I"../src/config" -I"../src/ASF/xmega/drivers/cpu" -I"../src/ASF/xmega/drivers/rtc" -I"../src/ASF/xmega/drivers/sleep" -I"../src/ASF/common/services/clock" -I"../src/ASF/common/services/sleepmgr"  -O0 -fdata-sections -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g3 -Wall -mmcu=atxmega32d4 -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\gcc\dev\atxmega32d4" -c -std=gnu99 -fno-strict-aliasing -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -mrelax -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler
src/ASF/xmega/drivers/cpu/ccp.o: ../src/ASF/xmega/drivers/cpu/ccp.s
	@echo Building file: $<
	@echo Invoking: AVR/GNU Assembler : 5.4.0
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -Wa,-gdwarf2 -x assembler-with-cpp -c -mmcu=atxmega32d4 -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\gcc\dev\atxmega32d4" -mrelax -DBOARD=USER_BOARD -I "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\include" -I "../src/ASF/common/boards/user_board" -I "../src/ASF/common/boards" -I "../src/ASF/xmega/utils/preprocessor" -I "../src/ASF/xmega/utils" -I "../src/ASF/common/utils" -I "../src" -I "../src/config" -I "../src/ASF/xmega/drivers/cpu" -I "../src/ASF/xmega/drivers/rtc" -I "../src/ASF/xmega/drivers/sleep" -I "../src/ASF/common/services/clock" -I "../src/ASF/common/services/sleepmgr"  -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -Wa,-g   -o "$@" "$<" 
	@echo Finished building: $<
	


src/ASF/xmega/drivers/cpu/%.o: ../src/ASF/xmega/drivers/cpu/%.s
	@echo Building file: $<
	@echo Invoking: AVR/GNU Assembler : 5.4.0
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -Wa,-gdwarf2 -x assembler-with-cpp -c -mmcu=atxmega32d4 -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\gcc\dev\atxmega32d4" -mrelax -DBOARD=USER_BOARD -I "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\include" -I "../src/ASF/common/boards/user_board" -I "../src/ASF/common/boards" -I "../src/ASF/xmega/utils/preprocessor" -I "../src/ASF/xmega/utils" -I "../src/ASF/common/utils" -I "../src" -I "../src/config" -I "../src/ASF/xmega/drivers/cpu" -I "../src/ASF/xmega/drivers/rtc" -I "../src/ASF/xmega/drivers/sleep" -I "../src/ASF/common/services/clock" -I "../src/ASF/common/services/sleepmgr"  -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -Wa,-g   -o "$@" "$<" 
	@echo Finished building: $<
	




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 5.4.0
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="WRL_Sensorknoten.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atxmega32d4 -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\XMEGAD_DFP\1.0.32\gcc\dev\atxmega32d4" -Wl,--relax  
	@echo Finished building target: $@
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "WRL_Sensorknoten.elf" "WRL_Sensorknoten.hex"
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "WRL_Sensorknoten.elf" "WRL_Sensorknoten.eep" || exit 0
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "WRL_Sensorknoten.elf" > "WRL_Sensorknoten.lss"
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "WRL_Sensorknoten.elf" "WRL_Sensorknoten.srec"
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "WRL_Sensorknoten.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "WRL_Sensorknoten.elf" "WRL_Sensorknoten.a" "WRL_Sensorknoten.hex" "WRL_Sensorknoten.lss" "WRL_Sensorknoten.eep" "WRL_Sensorknoten.map" "WRL_Sensorknoten.srec" "WRL_Sensorknoten.usersignatures"
	