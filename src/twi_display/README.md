# TWI Example w/ LCD 1602

> I recommend reading ![A Basic Guide to I2C](https://www.ti.com/lit/an/sbaa565/sbaa565.pdf) from Texas Instruments

Formula for SCL frequency from `25.3.4.2 TWI Master Operation`. Before starting
TWI as master, we have to set the baud rate and solve for BAUD.

```math
f_{SCL} = \frac{f_{CLK\_PER}}{10 + 2 BAUD + f_{CLK\_PER} * T_{RISE}}
```

To solve for BAUD, just plug in the values:
- CPU Clock is set to 10 MHz
- SCL is 100 kHz because we're using standard mode
- tRISE is from `31.10 I/O Pin Characteristics`

```math
\begin{multline}
\frac{f_{CLK\_PER}}{{f_{SCL}}} = 10 + 2 BAUD + f_{CLK\_PER} * T_{RISE} \\
\frac{10 MHz}{100 kHz} = 10 + 2 BAUD + 10 MHz * 2.5 ns \\
BAUD = 44.9875 \approx 45
\end{multline}
```


Minimum pullup can be calculated with the following formula.
Values taken from `Table 31-18. TWI - Timing Characteristics`.

```math
R_{P}(min) = \frac{VCC - V_{OL}(max)}{I_{OL}}
           = \frac{3.3-0.4}{3 mA}
           \approx 967 Ω
```