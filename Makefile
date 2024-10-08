# ${proj} ----> Name of project (e.g. blinky)
# ${atpack} --> Path to unzipped atpack (e.g. ~/atpack)
# ${port} ----> Port used by UPDI programmer

PROJECT_BUILD = ./src/${proj}/build

all:
	${MAKE} build proj=blinky atpack=~/atpack
	${MAKE} build proj=usart_temp_sensor atpack=~/atpack

build:
	mkdir -pv ${PROJECT_BUILD}
	avr-gcc -Wall -mmcu=attiny202 -B ${atpack}/gcc/dev/attiny202 -isystem ${atpack}/include -Os -o ${PROJECT_BUILD}/${proj}.elf ./src/${proj}/main.c
	avr-objcopy -O ihex ${PROJECT_BUILD}/${proj}.elf ${PROJECT_BUILD}/${proj}.hex
	avr-objdump -P mem-usage ${PROJECT_BUILD}/${proj}.elf

list:
	mkdir -pv ${PROJECT_BUILD}/list
	avr-gcc -S -mmcu=attiny202 -B ${atpack}/gcc/dev/attiny202 -isystem ${atpack}/include -Os -o ${PROJECT_BUILD}/list/${proj}.s ./src/${proj}/main.c
	avr-gcc -g -mmcu=attiny202 -B ${atpack}/gcc/dev/attiny202 -isystem ${atpack}/include -Os -o ${PROJECT_BUILD}/list/${proj}.elf ./src/${proj}/main.c
	avr-objdump -S -l ${PROJECT_BUILD}/list/${proj}.elf > ${PROJECT_BUILD}/list/${proj}.lst

upload: ${PROJECT_BUILD}/${proj}.hex
	avrdude -vvv -c serialupdi -b 230400 -P ${port} -p t202 -U flash:w:${PROJECT_BUILD}/${proj}.hex:i

hexdump: ${PROJECT_BUILD}/${proj}.hex
	avr-objdump -b ihex -s ${PROJECT_BUILD}/${proj}.hex

fuses: ${PROJECT_BUILD}/${proj}.hex
	 avrdude -vvv -c serialupdi -b 230400 -P ${port} -p t202 -U fuses:r:${PROJECT_BUILD}/${proj}.hex:a

clean:
	rm -rf ${PROJECT_BUILD}