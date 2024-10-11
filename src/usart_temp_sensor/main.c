#define F_CPU 10000000UL
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float) BAUD_RATE)) + 0.5)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>


// Declaring function prototypes as static saves 24 bytes
static void USART0_init(void);
static char USART0_readChar(void);
static void USART0_sendChar(char c);
static void USART0_sendString(char* str);
static int16_t read_temperature_sensor(void);
static void reverse(char* str, uint8_t length);
static char* itoa(int16_t num, char* str);
static char capitalize(char c);
static void LED_init(void);
static void LED_on(void);
static void LED_off(void);


char sensor_buffer[20];

// Every time, PA1 transitions from a high-to-low state (by pressing a button),
// send on-chip temperature sensor data to PC.
ISR(PORTA_PORT_vect) {
     // Check if the interrupt was caused by PA1
    if (PORTA.INTFLAGS & PIN1_bm) {
        int16_t temp = read_temperature_sensor();
        USART0_sendString("Temperature (C): ");
        USART0_sendString(itoa(temp, sensor_buffer));
        USART0_sendString("\r\n");
        _delay_ms(100);

        // Clear the interrupt flag
        PORTA.INTFLAGS = PIN1_bm;
    }
}

int main(void) {
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);
   
    char pc_char;

    _delay_ms(10);
    USART0_init();
    LED_init();


    // Set Pin 1 in Port A as input
    PORTA.IN = PIN1_bm;

    // Enable internal pull-up resistor for PA1
    // See 16.5.11 and Table 31-15. I/O Pin Characteristics
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;

    // Enable global interrupts
    sei();

    while (1) {
        pc_char = capitalize(USART0_readChar());        

        // To turn LED on or off send "Y" or "N" to USB port
        if (pc_char == 'Y') {
            LED_on();
            USART0_sendString("Turning LED on\r\n");

            // Debounce delay
            _delay_ms(100);
        } else if (pc_char == 'N') {
            LED_off();
            USART0_sendString("Turning LED off\r\n");

            // Debounce delay
            _delay_ms(100);
        }
    }
}

void USART0_init(void) {
    PORTA.DIR &= ~PIN7_bm; // RXD
    PORTA.DIR |= PIN6_bm; // TXD

    USART0.BAUD = (uint16_t) USART0_BAUD_RATE(9600);
    USART0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}


char USART0_readChar(void) {
    while (!(USART0.STATUS & USART_RXCIF_bm));
    return USART0.RXDATAL;
}


void USART0_sendChar(char c) {
    while (!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = c;
}


void USART0_sendString(char* str) {
    for (size_t i = 0; i < strlen(str); i++) {
        USART0_sendChar(str[i]);
    }
}

/*
 * Read the on-chip temperature sensor according to 29.3.2.6
 */
int16_t read_temperature_sensor(void) {
    // 1.  Configure the internal voltage reference to 1.1V 
    //     by configuring the in VREF peripheral.
    VREF.CTRLA = VREF_ADC0REFSEL_1V1_gc;

    // 2.  Select the internal voltage reference by writing 
    //     the REFSEL bits in ADCn.CTRLC to 0x0.
    ADC0.CTRLC = ADC_PRESC_DIV32_gc | ADC_REFSEL_INTREF_gc | ADC_SAMPCAP_bm;

    // 3. Select the ADC temperature sensor channel by configuring the
    //    MUXPOS register (ADCn.MUXPOS). This enables the temperature sensor
    ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;

    /* 
     * 4. In ADCn.CTRLD Select INITDLY ≥ 32 µs × fCLK_ADC
     *
     * Initialization Delay before first samples ≥  32 µs * (10 MHz / 32 prescaler division factor)
     * Initialization Delay before first samples ≥ 10
     * 
     * 29.5.4 Control D lists the following delay counts: 0, 16, 32, 64, 128, 256
     * so we choose 16.
     */
    ADC0.CTRLD = ADC_INITDLY_DLY16_gc;

    // 5. In ADCn.SAMPCTRL Select SAMPLEN ≥ 32 µs × fCLK_ADC
    ADC0.SAMPCTRL = 10;

    // Enable ADC
    ADC0.CTRLA = ADC_ENABLE_bm;
    // Start conversion for a single measurement
    ADC0.COMMAND = ADC_STCONV_bm;
    // Wait for conversion to complete
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));

    // Read signed value from signature row
    int8_t sigrow_offset = SIGROW.TEMPSENSE1;
    // Read unsigned value from signature row
    uint8_t sigrow_gain = SIGROW.TEMPSENSE0;
    // ADC conversion result with 1.1 V internal reference
    uint32_t temp = ADC0.RES - sigrow_offset;
    // Result might overflow 16 bit variable (10bit+8bit)
    temp *= sigrow_gain;
    // Add 1/2 to get correct rounding on division below
    temp += 0x80;
    // Divide result to get Kelvin 
    temp >>= 8;
    // Convert to Celsius
    temp -= 273;

    return ((int16_t) temp);
}


void reverse(char* str, uint8_t length) {
    uint8_t start = 0;
    uint8_t end = length - 1;
    while (start < end) {
        // Swap characters
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}


char* itoa(int16_t num, char* str) {
    uint8_t i = 0;

    // Handle 0 case explicitly
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Process each digit
    while (num != 0) {
        uint8_t digit = num % 10;
        // Convert digit to ASCII character
        str[i++] = digit + '0';

        num /= 10;
    }

    // Null-terminate the string
    str[i] = '\0';

    // The digits are in reverse order, so reverse the string
    reverse(str, i);

    return str;
}

char capitalize(char c) {
  uint8_t ascii_dec = (uint8_t) c;
  
  if (ascii_dec > 96 && ascii_dec < 123) {
    ascii_dec -= 32;
    return ((char) ascii_dec);
  }
  
  return c;
} 


void LED_init(void) {
    // Use PA2 as output for red LED
    PORTA.DIR |= PIN2_bm;
}


void LED_on(void) {
    PORTA.OUT |= PIN2_bm;
}


void LED_off(void) {
    PORTA.OUTCLR = PIN2_bm;
}
