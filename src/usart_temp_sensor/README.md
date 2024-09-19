# USART Example w/ On-Chip Temperature Sensor

A basic USART example that transfers on-chip temeprature sensor data to your PC's terminal every
1 second. The program uses up 1916 bytes, or 94% of flash memory!

```bash
> cat /dev/ttyUSB0
Temperature (C): 34

Temperature (C): 34

Temperature (C): 34

Temperature (C): 34
```

## Programming

Build the project then upload the hex file using avrdude

```
avrdude -vvv -c serialupdi -b 230400 -P /dev/ttyUSB0 -p t202 -U flash:w:your_build_file.hex:a
```

## Connections

Using a USB to TTL adapter, make the following connections to the dev board:
- Power it via USB-C **or** connect 3.3V/5V to VDD/VCC 
- GND to GND pin
- RXD to PA6 pin (TXD)
- TXD to PA7 pin (RXD)

Connect the adapter to your PC then run the following command

```bash
cat /dev/ttyUSB0
```

or

```bash
screen /dev/ttyUSB0 9600
```