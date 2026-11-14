// DSPIC33EP512GM604 Configuration Bit Settings

// 'C' source line config statements

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config BOREN = ON               //  (BOR is enabled)
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS256         // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128            // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (XT Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration (Allow only one reconfiguration)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock switching is enabled,Fail-safe Clock Monitor is disabled)

// FOSCSEL
#pragma config FNOSC = PRI              // Oscillator Source Selection (Primary Oscillator (XT, HS, EC))
#pragma config PWMLOCK = OFF            // PWM Lock Enable bit (PWM registers may be written without key sequence)
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include "xc.h"
#include "AuK.h"
#include "AuK_config.h"
#include "AUK_structures.h"
#include <libpic30.h> // Needed for delay32

void init_clock_signal(void)
{
    RCONbits.SWDTEN = 0; // Disable Watchdog Timer

    // This chip is taged with I-Temp stamp (-I/SP) then the temperature range
    // is -40ºC to 85ºC which implies a maximum of 70 MIPS (Fosc = 140 MHz)
    // Chapter 33.1 DC characteristics in dsPIC33EP512GM604 manual.
    
    // We take a conservative configuration
    
    // Fosc = Fin*M/(N1*N2), Fcy = Fosc/2
    // Fosc = 7.3728*65/(2*2) = 119.808 MHz
    // Fcy = Fosc/2 = 59.904 MHz

    // Configure PLL prescaler, PLL postscaler and PLL divisor

    PLLFBDbits.PLLDIV = 63; // M = PLLDIV + 2 = 65 -> PLLDIV = 65 - 2 = 63
    CLKDIVbits.PLLPOST = 0; // N2 = 2 (Output/2)
    CLKDIVbits.PLLPRE = 0; // N1 = 2 (Input/2)
    
    // clock switching to incorporate PLL
    __builtin_write_OSCCONH(0x03); // Initiate Clock Switch to Primary
    __builtin_write_OSCCONL(0x01); // Start clock switching

    while (OSCCONbits.COSC != 0b011); // Wait for Clock switch to occur
    while (OSCCONbits.LOCK != 1) {}; // Wait for PLL to lock (If LOCK = 1 -> PLL start-up timer is satisfied)

}

void init_ports(){
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB7 = 0;
}


//----------------------------------------------------------------------------

void P1 (void)
{
    int periodo = 10;
    int hora_despertar;
    int x;
    
    hora_despertar = clock();
    while(1)
    {
        for(x=1; x<= 500; x++)
        {
            PORTBbits.RB8=!LATBbits.LATB8;
        }
        hora_despertar += periodo;
        delay_until(hora_despertar);
    }
}

void P2 (void)
{
    while(1)
    {
        PORTBbits.RB7=!LATBbits.LATB7;
    }
}

int main(void) 
{
    init_clock_signal();
    init_ports();
    
    int x;
    init_AuK(59.904E6, 0.02); //20ms
    
    x = create_task(__builtin_tblpage(P1), __builtin_tbloffset(P1), 200, 2);
    x = create_task(__builtin_tblpage(P2), __builtin_tbloffset(P2), 200, 1);
    
    start_AuK();
    Sleep();

    return 0;
}
