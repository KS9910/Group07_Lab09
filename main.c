#include  <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#include  <stdint.h>

#include "tm4c123gh6pm.h"
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

void I2C_Write(uint8_t slave_addr, uint8_t reg, uint16_t data) {
    I2C0_MSA_R = (slave_addr << 1) & 0xFE;  // Set I2C Master Slave Address (write mode)

    // Set data to be sent (DAC register and data)
    I2C0_MDR_R = reg;                       // DAC register pointer
    I2C0_MCS_R = 3;                         // Send Start and Run conditions

    // Wait for transmission completion and check errors
    while (I2C0_MCS_R & 1);                 // Wait for transmission to complete
    if (I2C0_MCS_R & 0xE) return;           // Return on error

    I2C0_MDR_R = (data >> 8) & 0xFF;        // Send the MSB first
    I2C0_MCS_R = 1;                         // Run without start/stop

    while (I2C0_MCS_R & 1);                 // Wait for transmission to complete
    if (I2C0_MCS_R & 0xE) return;           // Return on error

    I2C0_MDR_R = data & 0xFF;               // Send the LSB
    I2C0_MCS_R = 5;                         // Run and Stop conditions

    while (I2C0_MCS_R & 1);                 // Wait for transmission to complete
    if (I2C0_MCS_R & 0xE) return;           // Return on error
}


