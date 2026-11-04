void init_adc(void)
{
    // Set In/Out pins
    
    TRISAbits.TRISA0 = ; //AN0 (RA0 as input)
        
    ANSELAbits.ANSA0 = ;   // AN0 analog
    
    
    AD1CON1bits.AD12B = ;  // 12 Bits single channel operation
    AD1CON1bits.ADSIDL = ; // Discontinues module operation when device enters Idle mode.
    AD1CON1bits.FORM = ;   // Data output format bits: integer (0000 dddd dddd dddd)
    
    // Clearing SAMP bits ends sampling and stars conversion
    AD1CON1bits.SSRCG = ;  
    AD1CON1bits.SSRC = ;
    
    AD1CON1bits.ASAM = ;   // Sampling begins when SAMP bit is set
    
    // Voltage reference configuration bits
    AD1CON2bits.VCFG = ;   // VrefH = AVDD, VrefL = AVSS
    
    AD1CON2bits.CSCNA = ;  // Do not scan inputs
    
    // Generates interrupt after completion of every sample/conversion operation
    // when ADDMAEN = 0 (see later)
    AD1CON2bits.SMPI = ;
    
    // Buffer fill mode (Does not affect the sampling in this case)
    // 16 bufffers whill be used if more than one conversion per interrupt
    // is used
    AD1CON2bits.BUFM = ;
    
    AD1CON2bits.ALTS = ;   // Always select MUXA
    
    AD1CON3bits.ADRC = ;   // Clock derived from system clock
    
        
    AD1CON3bits.ADCS = ; // MUY IMPORTANTE
    
    // Conversion results are stored in the ADC1BUF0 through ADC1BUFF registers
    // DMA will no used.
    AD1CON4bits.ADDMAEN = ; 
    
    AD1CHS0bits.CH0SA = ; // Channel 0 positive input is AN0
    AD1CHS0bits.CH0NA = ; // Channel 0 negative input is Vrefl
        
    IPC3bits.AD1IP = ;
    IFS0bits.AD1IF = ;
    IEC0bits.AD1IE = ;
    
    AD1CON1bits.ADON = ; //Start operation    
}
