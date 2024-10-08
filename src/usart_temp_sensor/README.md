# USART Example w/ On-Chip Temperature Sensor

A basic USART example that transfers on-chip temperature sensor data to your PC's terminal every
time you press a push button.

```bash
> cat /dev/ttyUSB0
Temperature (C): 34

Temperature (C): 34

Temperature (C): 34

Temperature (C): 34
```

If your sensor doesn't seem to output the right values, this could be the issue (from Errata).

![Screenshot (23)](https://github.com/user-attachments/assets/72a67ba8-01b6-484c-807e-d6d94cbd01d3)


## Programming

See [programming section](https://github.com/AnOrdinaryUsername/attiny202-dev-board/blob/master/README.md#programming).

## Connections

![USART Button Schematic](/docs/images/USART_Button.png)

Using a USB to TTL adapter, make the following connections to the dev board:
- Power it via USB-C **or** connect 3.3V/5V to VDD/VCC 
- GND to GND pin
- RXD to PA6 pin (TXD)
- TXD to PA7 pin (RXD)

On the breadboard with a push button:
- PA1 to any button pin
- GND to button pin next to PA1

Connect the adapter to your PC then run the following command

```bash
cat /dev/ttyUSB0
```

or

```bash
screen /dev/ttyUSB0 9600
```
