#include <stdint.h>
#include <stdbool.h>
#include <math.h>        // For sine function
#include "tm4c123gh6pm.h"

#define PI 3.14159265358979323846
#define FREQ 1000         // Desired frequency of the sine wave in Hz
#define SAMPLE_RATE 10000 // Number of samples per second
#define Slave_ADDRESS 0x51  // Assume DAC I2C address is 0x76 (change if needed)
#define DAC_reg 0x40  // DAC register to write

void I2C_GPIO_CONFG(void)
{
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;  // enable clock for I2C
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;  //enable clock for GPIO
    GPIO_PORTB_AFSEL_R |= (1 << 2) | (1 << 3); //Enable alternate functions for PB2 (SCL) and PB3 (SDA)
    GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB3_I2C0SDA | GPIO_PCTL_PB2_I2C0SCL;  // Set PCTL for PB2 as SCL and PB3 as SDA
    GPIO_PORTA_DEN_R |= (1 << 2) | (1 << 3);     // Enable digital I/O for PB2 and PB3
    GPIO_PORTB_ODR_R |= (1 << 3); // open-drain operation for PB3(SDA)
    I2C0_MCR_R= 0x00000010; // Initialize the I2C Master
    I2C0_MTPR_R= 9;  // TPR is 9
//    I2C0_MSA_R = 0x00000076; //Master address for transmit
//    I2C0_MDR_R= 0X000000C3;  // Slave address which need to send initially
//    I2C0_MCS_R = 0x00000007;
}

void I2C_Write(uint8_t slave_addr, uint8_t reg, uint8_t data) {
    I2C0_MSA_R = (slave_addr << 1) & 0xFE;  // Set I2C Master Slave Address (write mode)

    // Set data to be sent (DAC register and data)
    I2C0_MDR_R = reg;                       // DAC register pointer
    I2C0_MCS_R = 3;                         // Send Start and Run conditions

    // Wait for transmission completion and check errors
    while (I2C0_MCS_R & 1);                 // Wait for transmission to complete
    if (I2C0_MCS_R & 0xE) return;           // Return on error

//    I2C0_MDR_R = (data >> 8) & 0xFF;        // Send the MSB first
//    I2C0_MCS_R = 1;                         // Run without start/stop
//
//    while (I2C0_MCS_R & 1);                 // Wait for transmission to complete
//    if (I2C0_MCS_R & 0xE) return;           // Return on error

    I2C0_MDR_R = data;               // Send the LSB
    I2C0_MCS_R = 5;                         // Run and Stop conditions

    while (I2C0_MCS_R & 1);                 // Wait for transmission to complete
    if (I2C0_MCS_R & 0xE) return;           // Return on error
}

void generate_sine_wave() {
    uint8_t sine_wave[100];  // Array to store sine wave values
    int i;
    for (i = 0; i < 100; i++) {
        // Generate sine values between 0 and 255 for an 8-bit DAC
        sine_wave[i] = (uint8_t)(127.5 * (1 + sin(2 * PI * i / 100)));
    }

    while (1) {
        for (i = 0; i < 100; i++) {
            I2C_Write(Slave_ADDRESS, DAC_reg, sine_wave[i]);  // Send each value to DAC
        }
    }
}

int main(void) {
    I2C_GPIO_CONFG();                  // Configure I2C GPIO
    generate_sine_wave(); // Generate triangular wave
    return 0;
}
