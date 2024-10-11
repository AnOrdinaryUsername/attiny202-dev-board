# USART Example w/ On-Chip Temperature Sensor

A basic USART example that transfers on-chip temperature sensor data to your PC's terminal every
time you press a push button.

```bash
> cat /dev/ttyUSB0
Temperature (C): 27

Turning LED on

Turning LED off
```

If your sensor doesn't seem to output the right values, this could be the issue (from Errata).

![Screenshot (23)](https://github.com/user-attachments/assets/72a67ba8-01b6-484c-807e-d6d94cbd01d3)


## Programming

See [programming section](https://github.com/AnOrdinaryUsername/attiny202-dev-board/blob/master/README.md#programming).

## Connections

![USART Button Schematic](/docs/images/USART_Button.png)

Using a USB to Serial Adapter, make the following connections to a breadboard:
- Connect 3.3V/5V to Breadboard +
- GND to Breadboard -

On the breadboard:
- Add a button + LED with 470Ω resistor
- Breadboard - to any button pin and cathode of LED

With the dev board connect:
- VDD to Breadboard +
- GND to Breadboard -
- PA6 pin (TXD) to RXD of USB to Serial Adapter
- PA7 pin (RXD) to TXD of USB to Serial Adapter
- PA1 on button pin (near the one that was connected to ground)
- PA2 to anode of LED

Connect the adapter to your PC then run the following command

```bash
cat /dev/ttyUSB0
```

or

```bash
screen /dev/ttyUSB0 9600
```

To turn LED on or off, run
```
echo "y" > /dev/ttyUSB0
echo "n" > /dev/ttyUSB0
```
