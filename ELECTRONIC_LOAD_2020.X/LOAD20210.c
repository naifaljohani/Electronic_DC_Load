#include "ADC.h"
#include "GLCD.h"
#include "LDC_ROT.h"
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "string.h"
#include <stdint.h>
#include <stdbool.h>



#define _XTAL_FREQ             16000000
#define Encoder_DT             PORTAbits.RA0 
#define Encoder_CLOCK          PORTAbits.RA1
#define Button_Press           PORTAbits.RA3
#define UP                     PORTAbits.RA4
#define DOWN                   PORTAbits.RA5


#define LED0                   LATDbits.LATD0 
#define LED1                   LATDbits.LATD1 

#define X_1    LATCbits.LATC0
#define X_2    LATCbits.LATC1
#define X_3    LATCbits.LATC2
#define X_4    LATCbits.LATC3
#define Y_1    PORTCbits.RC4
#define Y_2    PORTCbits.RC5
#define Y_3    PORTCbits.RC6
#define Y_4    PORTCbits.RC7

//#define Keypad_PORT               LATC
#define Keypad_PORT_Direction     TRISC 

void InitKeypad(void);
char switch_press_scan(void);

char naif[17]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
char UPDOWN[17]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
unsigned char hello[25]="naif hamdi aljohani   ";
char Key = 'n';
 uint16_t dac_out =0;

 int16_t  count = 520;
uint16_t   current = 0;
 unsigned char print_line = 3;
 int currentStateCLK;
 int previousStateCLK;
 
 
void interrupt_init()
{
  GIE   = 1  ; 
  IOCIE = 1 ;
  IOCAP3=1;
}
void interrupt isr()
{
   
   if (IOCAF3 == 1) // button pressed 
   {
     IOCAF3 =0;    //  clear flag   
    
     count=0;
   
     
   }
     
}

char keypad_scanner(void)  
{           
            X_1 = 0; X_2 = 1; X_3 = 1; X_4 = 1;    
            if (Y_1 == 0) { __delay_ms(100); while (Y_1==0); return '1'; }
            if (Y_2 == 0) { __delay_ms(100); while (Y_2==0); return '2'; }
            if (Y_3 == 0) { __delay_ms(100); while (Y_3==0); return '3'; }
            if (Y_4 == 0) { __delay_ms(100); while (Y_4==0); return 'A'; }
            
            X_1 = 1; X_2 = 0; X_3 = 1; X_4 = 1;    
            if (Y_1 == 0) { __delay_ms(100); while (Y_1==0); return '4'; }
            if (Y_2 == 0) { __delay_ms(100); while (Y_2==0); return '5'; }
            if (Y_3 == 0) { __delay_ms(100); while (Y_3==0); return '6'; }
            if (Y_4 == 0) { __delay_ms(100); while (Y_4==0); return 'B'; }
            
            X_1 = 1; X_2 = 1; X_3 = 0; X_4 = 1;    
            if (Y_1 == 0) { __delay_ms(100); while (Y_1==0); return '7'; }
            if (Y_2 == 0) { __delay_ms(100); while (Y_2==0); return '8'; }
            if (Y_3 == 0) { __delay_ms(100); while (Y_3==0); return '9'; }
            if (Y_4 == 0) { __delay_ms(100); while (Y_4==0); return 'C'; }
           
            X_1 = 1; X_2 = 1; X_3 = 1; X_4 = 0;    
            if (Y_1 == 0) { __delay_ms(100); while (Y_1==0); return '*'; }
            if (Y_2 == 0) { __delay_ms(100); while (Y_2==0); return '0'; }
            if (Y_3 == 0) { __delay_ms(100); while (Y_3==0); return '#'; }
            if (Y_4 == 0) { __delay_ms(100); while (Y_4==0); return 'D'; }
            
    return ;                   
} 
void InitKeypad(void)
{
            //Keypad_PORT                = 0x00;        // Set Keypad port pin values zero       
            OPTION_REGbits.nWPUEN =0;
            Keypad_PORT_Direction = 0b11110000;      // Last 4 pins input, First 4 pins output 
            WPUC = 0b11110000;
            ANSELC=0;
}
void main(void)
{
  ///  IOCIF  , IOCAFx            ///Interrupt-on-Change Interrupt Flag bit  
  OSCILLATOR_Initialize();  
  //interrupt_init();
  DAC_CONFIG();                  // DAC ON PIN RA2 !
  
  
  
  
  TRISA=0b00001011;
  ANSELA=0;
  INLVLA=0;
  
  ADC_Initialize();
  
  TRISD=0;
  ANSELD=0;
  // WPUD =0XFF;
 
  TRISB=0;
  ANSELB=0; 
  ODCONB=0;
  SLRCONB=0;
 /// __delay_ms(500);
 
     InitKeypad();
  ////////////////////////// greeting maseage ///// 
    LCD_Init( );
    LCD_EnableGraphics();
    LCD_ClearGraphics();
    LCD_FillScreenGraphics(des);
    __delay_ms(1000);
    LCD_ClearGraphics();
    LCD_DisableGraphics();  
    
    
    previousStateCLK = Encoder_CLOCK;
 
   
   
  while(1)
  {
    current= ( ADC_GetConversion(channel_AN7)*5)-140; 
    naif[3]=' ';
    sprintf(naif,"%d", current);
    
    LCD_TextDisplayString( 3, "Current   in mA:");
    LCD_TextDisplayString( 4, naif);   
    
      
 // encoder input ///     
    if(Button_Press==0)
    {
     count =520;
    
     
    }
      
      if (Encoder_CLOCK == 0) // button pressed 
   {
    __delay_ms(10);
     currentStateCLK = Encoder_CLOCK;
    
  
     // If the inputDT state is different than the inputCLK state then 
     // the encoder is rotating counterclockwise
     if (Encoder_DT != currentStateCLK) { 
       count  --;
       if (count <520)
     {
         count  =520;
     }
       LED0= 0;
       LED1= 1;
       
     } else {
       // Encoder is rotating clockwise
       count  ++;
      
        LED0= 1;
        LED1= 0;
       
     }
     previousStateCLK = currentStateCLK;   // Update previousStateCLK with the current state
   
   }
    
    
    naif[3]=' ';
    sprintf(naif,"%d", (count *5)+1);
    LCD_TextDisplayString( 1, "DAC out   in mv:");
    LCD_TextDisplayString( 2, naif); 
    
           
           
   UPDOWN[1]=switch_press_scan();
  
     
      //////////////////////////////
    
     DAC1_Load10bitInputData(count*64);
      
    //  DAC1_Load10bitInputData ((naif[1]-48)*5120);
      
   
     if ( UPDOWN[1] == 'A')
     {
        count++; 
        UPDOWN[1]=' ';
     }
      if (UPDOWN[1] == 'B')
     {
        count--; 
        UPDOWN[1]=' ';
     }
   
    //  LCD_TextDisplayString(print_line, naif);
      
      
      
      }
        
        
  
   }   
    

 

 
 

 char switch_press_scan(void)                       // Get key from user
{
            char key = 'n';              // Assume no key pressed
            while(key=='n')              // Wait untill a key is pressed
            key = keypad_scanner();   // Scan the keys again and again
            return key;                  //when key pressed then return its value
}
  

 