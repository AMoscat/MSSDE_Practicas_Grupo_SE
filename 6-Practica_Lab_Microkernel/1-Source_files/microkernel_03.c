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
#include <stdio.h>

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

void init_uart1(void)
{
     /* Specified pins for UART1 */
    
    /* RX RPI44 */
    RPINR18bits.U1RXR = 0b101100;
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
    
    /* interruption RX config */
    //U1STAbits.URXISEL = 0b00;
    //IPC2bits.U1RXIP = 5;
    //IFS0bits.U1RXIF = 0;
    //IEC0bits.U1RXIE = 1;
    
    __C30_UART = 1; // Redirects printf output to UART1
}


//----------------------------------------------------------------------------

#define NUM_PROCESOS 5

Tsemaphore print_sem;

void proceso_periodico (void)
{   
    int id = give_me_my_id();
    int periodo = 50 + 50 * id;
    int hora_despertar;
    
    hora_despertar = clock();
    while(1)
    {
        wait(&print_sem);
        printf("My id is: %d\n\r", id);
        signal(&print_sem);
        
        hora_despertar += periodo;
        delay_until(hora_despertar);
    }
}

int main(void) 
{
    init_clock_signal();
    init_ports();
    init_uart1();
    
    printf("Start\n\r");
    
    init_semaphore(&print_sem, 1);
    
    int x;
    init_AuK(59.904E6, 0.02); //20ms
    
    for(int i=0; i<NUM_PROCESOS; i++)
    {
        x = create_task(__builtin_tblpage(proceso_periodico), __builtin_tbloffset(proceso_periodico), 200, NUM_PROCESOS-i);
    }
    
    start_AuK();
    Sleep();

    return 0;
}
