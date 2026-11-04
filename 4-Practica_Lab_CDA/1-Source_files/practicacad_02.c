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


void init_adc(void)
{
    // Set In/Out pins
    
    TRISAbits.TRISA0 = 1; //AN0 (RA0 as input)
    TRISAbits.TRISA1 = 1; //AN1 (RA1 as input)
        
    ANSELAbits.ANSA0 = 1;   // AN0 analog
    ANSELAbits.ANSA1 = 1;   // AN1 analog
    
    
    AD1CON1bits.AD12B = 1;  // 12 Bits single channel operation
    AD1CON1bits.ADSIDL = 0; // Discontinues module operation when device enters Idle mode.
    AD1CON1bits.FORM = 0;   // Data output format bits: integer (0000 dddd dddd dddd)
    
    // Clearing SAMP bits ends sampling and stars conversion
    AD1CON1bits.SSRCG = 0;  
    AD1CON1bits.SSRC = 0;
    
    AD1CON1bits.ASAM = 0;   // Sampling begins when SAMP bit is set
    
    // Voltage reference configuration bits
    AD1CON2bits.VCFG = 0;   // VrefH = AVDD, VrefL = AVSS
    
    AD1CON2bits.CSCNA = 0;  // Do not scan inputs
    
    // Generates interrupt after completion of every sample/conversion operation
    // when ADDMAEN = 0 (see later)
    AD1CON2bits.SMPI = 0;
    
    // Buffer fill mode (Does not affect the sampling in this case)
    // 16 bufffers whill be used if more than one conversion per interrupt
    // is used
    AD1CON2bits.BUFM = 0;
    
    AD1CON2bits.ALTS = 0;   // Always select MUXA
    
    AD1CON3bits.ADRC = 0;   // Clock derived from system clock
    
    // Tad = (ADCS+1)*Tcy
    // Fcy = 59.904 MHz:
    // Tcy = 16.69 ns
    // Tad MICROCHIP para 12 bits =  117.6 ns
    // (ADCS+1)*16,69 >= 117,6
    AD1CON3bits.ADCS = 7; // MUY IMPORTANTE
    
    // Conversion results are stored in the ADC1BUF0 through ADC1BUFF registers
    // DMA will no used.
    AD1CON4bits.ADDMAEN = 0; 
    
    AD1CHS0bits.CH0SA = 0; // Channel 0 positive input is AN0
    AD1CHS0bits.CH0NA = 0; // Channel 0 negative input is Vrefl
        
    IPC3bits.AD1IP = 5;
    IFS0bits.AD1IF = 0;
    IEC0bits.AD1IE = 1;
    
    AD1CON1bits.ADON = 1; //Start operation    
}

//----------------------------------------------------------------------------

volatile int adc_resultado = 0;
volatile int espera_adc = 1;

int leer_adc(int canal)
{
    AD1CHS0bits.CH0SA = canal;

    // Comienzo adquisicion con espera de 100us aprox
    AD1CON1bits.SAMP = 1;
    __delay32(6000);
    // Comienzo conversion
    AD1CON1bits.SAMP = 0;

    while(espera_adc)
    {
    }
    espera_adc = 1;

    // Devolver el resultado
    return adc_resultado;
}

int main(void) 
{
    init_clock_signal();
    init_uart1();
    init_adc();
    
    while(1)
    {
        int resultado_tension = leer_adc(0);
        int resultado_luminosidad = leer_adc(1);
        
        printf("tension: %d, luminosidad: %d\r\n", resultado_tension, resultado_luminosidad);

        // Esperar aproximadamente 1 s
        // 59904000 * 1
        delay32(59904000);
    }

    return 0;
}

void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void)
{
    // Leemos resultado y limpiamos flag
    adc_resultado = ADC1BUF0;
    IFS0bits.AD1IF = 0;
    espera_adc = 0;
}