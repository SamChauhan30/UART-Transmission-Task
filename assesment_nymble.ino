#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define CPU_frequency 16000000UL
#define BAUD 2400
#define BAUDRATESET CPU_frequency/16/BAUD-1
#define TIMEOUT_MS 100

void UART_Init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);  // Enable RX and TX via registers
    UCSR0C = (1 << USBS0) | (3 << UCSZ00); // 8 data bits, 2 stop bits
}

void UART_Transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;  // Send data
}

void UART_Send(const char *str) {
    while (*str) {
        UART_Transmit(*str++);
    }
    UART_Transmit('\0');
}

char UART_Receive(void) {
    while (!(UCSR0A & (1 << RXC0)));  // Wait for data to be received
    return UDR0;  
}

void UART_Receive(char *buffer, unsigned int max_length) {
    unsigned int i = 0;
    char received_char;
    unsigned int no_data_counter = 0;

    // Clear buffer
    while (UCSR0A & (1 << RXC0)) {
        (void)UDR0;
    }

    // Receive characters until timeout or max buffer is reached
    while (i < max_length - 1) {
        if (UCSR0A & (1 << RXC0)) {
            no_data_counter = 0;  
            received_char = UDR0;  // Read data

            // Append received character to buffer
            buffer[i++] = received_char;

            // Check for the null terminator to mark end of data
            if (received_char == '\0') {
                i--;  // Exclude the null terminator from the stored message
                break;
            }
        } else {
            _delay_ms(1);
            no_data_counter++;
            if (no_data_counter >= TIMEOUT_MS) {      //checking timeout
                break;
            }
        }
    }
    buffer[i] = '\0';  // Null character to terminate the string
}

int main(void) {
    UART_Init(BAUDRATESET);

    char received_data[1024];  // Buffer to store received data

    while (1) {
        // Step 1: Receive data from PC
        UART_Receive(received_data, sizeof(received_data));

        // Step 2: Debug print to confirm reception
        UART_Send("Received in microcontroller:\n");
        UART_Send(received_data);
        UART_Send("\n");

        // Step 3: Add a small delay to ensure PC is ready
        _delay_ms(100);

        // Step 4: Send the received data back to the PC
        UART_Send(received_data);

        // Clear the receive buffer to avoid carrying over old data
        memset(received_data, 0, sizeof(received_data));
    }

    return 0;

}
