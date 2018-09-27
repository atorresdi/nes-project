
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <driverlib/prcm.h>
#include <driverlib/sys_ctrl.h>
#include <driverlib/ioc.h>

#include <driverlib/gpio.h>
#include <driverlib/timer.h>

const uint32_t TIMER_TICKS_PER_USEC = 48;
const uint32_t DELAY_TIME_USEC = 1000*100; // 100 ms

void Startup();
void Timer_Init();
void GPIO_Init();

void Delay_Microsec(uint32_t time_us);

int main(void)
{
    Startup();
    Timer_Init();
    GPIO_Init();

    IOCPinTypeGpioOutput(10); // Configure DIO 0 as 'standard' output

    while (1)
    {
        GPIO_toggleDio(10);
        Delay_Microsec(DELAY_TIME_USEC);
    }

    return 0;
}

void Startup()
{
    // Power peripherals
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
    while((PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON));

    // Set clock source
    OSCClockSourceSet(OSC_SRC_CLK_MF | OSC_SRC_CLK_HF, OSC_XOSC_HF);
    while (OSCClockSourceGet(OSC_SRC_CLK_HF) != OSC_XOSC_HF)
    {
        OSCHfSourceSwitch();
    }
}

void Timer_Init()
{
    // Power timer
    PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER0);
    PRCMLoadSet();
    while(!PRCMLoadGet());

    // Setup timer0
    TimerConfigure(GPT0_BASE, TIMER_CFG_ONE_SHOT);
    TimerLoadSet(GPT0_BASE, TIMER_A, 0);
}

void GPIO_Init()
{
    // Power GPIO
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMLoadSet();
    while(!PRCMLoadGet());
}

void Delay_Microsec(uint32_t time_us)
{
    // Enable timer and wait for interrupt flag
    TimerLoadSet(GPT0_BASE, TIMER_A, time_us * TIMER_TICKS_PER_USEC);
    TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(GPT0_BASE, TIMER_A);
    while (!(TimerIntStatus(GPT0_BASE, false) & TIMER_TIMA_TIMEOUT));
}


