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
#pragma config WDTPOST = PS1024         // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR32            // Watchdog Timer Prescaler bit (1:128)
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
#include <libpic30.h> // Needed for delay32

void init_clock_signal(void)
{
    RCONbits.SWDTEN = 0; // Disable Watchdog Timer

    // This chip is taged with I-Temp stamp (-I/SP) then the temperature range
    // is -40ºC to 85ºC which implies a maximum of 70 MIPS (Fosc = 140 MHz)
    // Chapter 33.1 DC characteristics in dsPIC33EP512GM604 manual.
    
    // We take a conservative configuration
    
    // Configure Oscillator to operate the device at 40 Mhz
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



void init_uart1(void)
{
     /* Specified pins for UART1 */
    
    /* RX RPI44 */
    RPINR18bits.U1RXR = 0b10110;
    TRISBbits.TRISB12 = 1;
    
    /* TX RP43 */
    RPOR4bits.RP43R = 0b000001;
    TRISBbits.TRISB11 = 0;
    
    
    /* Configure UART */
    U1MODEbits.USIDL = 0;
    U1MODEbits.IREN = 0;
    U1MODEbits.UEN = 0b00;
    U1MODEbits.WAKE = 0;
    U1MODEbits.LPBACK = 0;
    U1MODEbits.ABAUD = 0;
    U1MODEbits.URXINV = 0;
    U1MODEbits.BRGH = 0;
    U1MODEbits.PDSEL = 0b00;
    U1MODEbits.STSEL = 0;
    
    
    U1STAbits.URXISEL = 0b00; //0x
    
    // U1BRG = (Fcy/(16*Baud_rate) -1)
    U1BRG = 390; // (59.904Mhz / (16*9600)-1)
           
    U1MODEbits.UARTEN = 1; // Enable UART operation
    U1STAbits.UTXEN = 1;    // Enable uart1 TX (must be done after UARTEN)
    
    /* It is needed to wait one transmision bit to ensure start bit detection
       in the connected receiver device.
       When 9600 Baud rate is selected it is necessary to wait 104 us */

    __delay32(5710); 
    
    //__C30_UART = 1; // Redirects printf output to UART1
}



//-----------------------------------------------------------------------------

void initTimer45(){
    T4CONbits.TSIDL = 1;
    T4CONbits.TGATE = 0;
    T4CONbits.TCS = 0;
    T4CONbits.T32 = 1;
    T4CONbits.TCKPS = 0b11;
    PR4 = 0xDC00;
    PR5 = 0x0011;
    IPC7bits.T5IP = 7;
    IFS1bits.T5IF = 0;
    IEC1bits.T5IE = 1;
    T4CONbits.TON = 1;
}

void init_ports(){
    TRISBbits.TRISB8 = 0;
}

void enviar_char(char caracter)
{
    while(U1STAbits.UTXBF == 1)
    {
    }
    
    U1TXREG = caracter;
}

int main(void) 
{
    init_clock_signal();
    init_uart1();
    //init_ports();
    //initTimer45();
    
    char caracter;
    while(1)
    {
        for(caracter='0'; caracter<='9'; caracter++)
        {
            enviar_char(caracter);
        }
        enviar_char('\n');
        enviar_char('\r');
    }

    return 0;
}

void __attribute__((interrupt(no_auto_psv))) _T5Interrupt(void){
    PORTBbits.RB8 = !LATBbits.LATB8;
    IFS1bits.T5IF = 0;
}
