#ifndef BARE_SERIAL_H
#define BARE_SERIAL_H

#include <avr/io.h>
#include <avr/interrupt.h>

// Define your custom buffer size here.
// Arduino uses 64 bytes. If you only receive short commands, 16 or 32 is enough.
#define RX_BUFFER_SIZE 50 

extern "C" void USART_RX_vect(void);

class BareSerial {
public:
    // Initialize the UART with a specific baud rate
    void begin(long baud) {
        // [FIX] Enable Double Speed Operation (U2X0)
        // This reduces the clock divisor from 16 to 8.
        // Required for 115200 baud on 16MHz Arduino to keep error < 2.5%.
        UCSR0A = (1 << U2X0);

        // Calculate the UBRR value (Baud Rate Register)
        // formula: (F_CPU / (16 * baud)) - 1
        uint16_t ubrr = (F_CPU / 8 / baud) - 1;

        // Set baud rate registers (High and Low bytes)
        UBRR0H = (unsigned char)(ubrr >> 8);
        UBRR0L = (unsigned char)ubrr;

        // Enable Receiver (RXEN0), Transmitter (TXEN0), and RX Interrupt (RXCIE0)
        UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

        // Set frame format: 8 data bits, 1 stop bit, no parity (8N1)
        // UCSR0C defaults to 8N1, but setting it explicitly is safer.
        // UCSZ01 + UCSZ00 = 8-bit
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
        
        // Enable global interrupts so our ISR can run
        sei(); 
    }

    // Send a single character
    void write(char data) {
        // Wait for the transmit buffer to be empty (UDRE0 bit)
        while (!(UCSR0A & (1 << UDRE0)));
        // Put data into buffer, sends the data
        UDR0 = data;
    }

    // Send a string (C-string)
    void print(const char* str) {
        while (*str) {
            write(*str++);
        }
    }

    // Check if data is available in the buffer
    bool available() {
        return _rxHead != _rxTail;
    }

    // Read one byte from the buffer
    char read() {
        if (_rxHead == _rxTail) return 0; // Buffer empty

        char data = _rxBuffer[_rxTail];
        // Advance tail and wrap around
        _rxTail = (_rxTail + 1) % RX_BUFFER_SIZE;
        return data;
    }

    // Friend function to allow the ISR to access private members
    friend void USART_RX_vect(void);

private:
    // Volatile is mandatory for variables shared with ISRs
    static volatile char _rxBuffer[RX_BUFFER_SIZE]; 
    static volatile uint8_t _rxHead;
    static volatile uint8_t _rxTail;
};

#endif