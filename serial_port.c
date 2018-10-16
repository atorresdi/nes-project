/*
 * serial_port.c
 *
 *  Created on: 15 oct. 2018
 *      Author: Alvaro
 */

#include <stdlib.h>

#include <driverlib/ioc.h>
#include <driverlib/gpio.h>
#include <driverlib/prcm.h>
#include <driverlib/timer.h>
#include <driverlib/uart.h>
#include <driverlib/sys_ctrl.h>

#include "CC2650STK.h"
#include "serial_port.h"

void Sep_Init()
{
    // Power serial interfaces
    PRCMPowerDomainOn(PRCM_DOMAIN_SERIAL); // TODO Should go to Startup code; also needed for I2C and SPI !
    while((PRCMPowerDomainStatus(PRCM_DOMAIN_SERIAL) != PRCM_DOMAIN_POWER_ON));

    // Power UART
    PRCMPeripheralRunEnable(PRCM_PERIPH_UART0);
    PRCMLoadSet();
    while(!PRCMLoadGet());

    // Map GPIOs to UART signals
    IOCPinTypeUart(UART0_BASE, IOID_UNUSED, Board_EB_UART_TX, IOID_UNUSED, IOID_UNUSED); // only tx pin is assigned
//    IOCPinTypeUart(UART0_BASE, IOID_UNUSED, Board_DP5_UARTTX, IOID_UNUSED, IOID_UNUSED); // only tx pin is assigned

    // Configure and enable UART
    UARTConfigSetExpClk(UART0_BASE, SysCtrlClockGet(), 115200,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    UARTEnable(UART0_BASE);

    Sep_Send("\f", sizeof("\f"));
}

void Sep_Send(char *buf, size_t count)
{
    for (size_t n = 0; n < count; n++)
    {
//        while (HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFF); // wait until there is space in the FIFO
        HWREG(UART0_BASE + UART_O_DR) = buf[n];
    }
}

