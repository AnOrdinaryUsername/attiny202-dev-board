#define F_CPU 10000000UL
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float) BAUD_RATE)) + 0.5)
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

void USART0_init(void) {
    PORTA.DIR &= ~PIN7_bm; // RXD
    PORTA.DIR |= PIN6_bm;  // TXD

    USART0.BAUD = (uint16_t) USART0_BAUD_RATE(9600);
    USART0.CTRLB |= USART_TXEN_bm;
}

void USART0_sendChar(char c) {
    while (!(USART0.STATUS & USART_DREIF_bm)) {
        ;
    }
    USART0.TXDATAL = c;
}

void USART0_sendString(char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
        USART0_sendChar(str[i]);
    }
}

/*
 * Read the on-chip temperature sensor according to 29.3.2.6
 */
int16_t read_temperature_sensor() {
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
     * Initialization Delay before first samples ≥  32 µs * (10 MHz / 32 prescale division factor)
     * Initialization Delay before first samples ≥ 10
     * 
     * 29.5.4 Control D lists the following delay counts: 0, 16, 32, 64, 128, 256
     * so we choose 16.
     */
     
    ADC0.CTRLD = ADC_INITDLY_DLY16_gc;
    // 5. In ADCn.SAMPCTRL Select SAMPLEN ≥ 32 µs × fCLK_ADC
    ADC0.SAMPCTRL = 10 ;
    
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

void reverse(char* str, int length) {
    int start = 0;
    int end = length - 1;
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
    int i = 0;

    // Handle 0 case explicitly
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Process each digit
    while (num != 0) {
        int digit = num % 10;
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

int main(void) {
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);
    char buffer[20];
    USART0_init();
    
    while (1) {
        int16_t temp = read_temperature_sensor();
        USART0_sendString("Temperature (C): ");
        USART0_sendString(itoa(temp, buffer));
        USART0_sendString("\r\n");
        _delay_ms(1000);
    }
}