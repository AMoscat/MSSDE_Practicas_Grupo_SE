/*
 * File:   main_practica_1_SE.c
 * Author: alumno
 *
 * Created on 17 de octubre de 2026, 13:25
 */


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
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = OFF             // PLL Lock Enable bit (Clock switch will not wait for the PLL lock signal.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (XT Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = ON            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FOSCSEL
#pragma config FNOSC = PRI              // Oscillator Source Selection (Primary Oscillator (XT, HS, EC))
#pragma config PWMLOCK = OFF            // PWM Lock Enable bit (PWM registers may be written without key sequence)
#pragma config IESO = ON              // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include "xc.h"

char mi_variable;


void init_clock_signal(void)
{
    RCONbits.SWDTEN = 0; // Disable Watchdog Timer

    // This chip is taged with I-Temp stamp (-I/SP) then the temperature range
    // is -40ºC to 85ºC which implies a maximum of 70 MIPS (Fosc = 140 MHz)
    // Chapter 33.1 DC characteristics in dsPIC33EP512GM604 manual.
    
    // We take a conservative configuration
    
    // Configure Oscillator to operate the device at 119.808 MHz
    // Fosc = Fin*M/(N1*N2), Fcy = Fosc/2
    // Fosc = 7.3728*XX/(X*X) = 119.808 MHz
    // Fcy = Fosc/2 = 59.904 MHz

    // Configure PLL prescaler, PLL postscaler and PLL divisor

    PLLFBDbits.PLLDIV = 63; // M = PLLDIV + 2 = XX -> PLLDIV = XX - 2 = XX
    CLKDIVbits.PLLPOST = 0; // N2 = X (Output/2)
    CLKDIVbits.PLLPRE = 0; // N1 = X (Input/2)
    
    // clock switching to incorporate PLL
    __builtin_write_OSCCONH(0x03); // Initiate Clock Switch to Primary
    __builtin_write_OSCCONL(0x01); // Start clock switching

    while (OSCCONbits.COSC != 0b011); // Wait for Clock switch to occur
    while (OSCCONbits.LOCK != 1); // Wait for PLL to lock (If LOCK = 1 -> PLL start-up timer is satisfied)

}

void initTimer1(){
    T1CONbits.TSIDL = 1;
    T1CONbits.TGATE = 0;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b11;
    PR1 = 23400;
    IPC0bits.T1IP = 7;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    T1CONbits.TON = 1;
}

void init_ports(){
    TRISBbits.TRISB8 = 0;
}

int main(void) 
{
    init_clock_signal();
    init_ports();
    initTimer1();
    
    while(1);

    return 0;
}

void __attribute__((interrupt(no_auto_psv))) _T1Interrupt(void){
    PORTBbits.RB8 = !LATBbits.LATB8;
    IFS0bits.T1IF = 0;
}
