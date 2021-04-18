/*
 * File:   main.c
 * Author: hesham
 *
 * Created on April 17, 2021, 2:56 AM
 */


#include <xc.h>
#include "config.h"
#include "lcd.h"

#define MOTD_S "Vault System"

void setup();
void get_input();

short letters = 4;

char const keyPadMatrix[] =
{
    '1','2','3',
    '4','5','6',
    '7','8','9',
    '*','0','#',
    0xFF
};
 

void __interrupt() interrupts() {
    if (INT0IF && INT0IE){
        //Lcd_Write_String("Key press INT0");
        get_input();
        INT0IF = 0;
    } else if (INT1IF && INT1IE) {
        get_input();
        INT1IF = 0;
    }
    else if (INT2IF && INT2IE) {
        get_input();
        INT2IF = 0;
    } else if (INT3IF && INT3IE) {
        get_input();
        INT3IF = 0;
    }
    letters--;
    return;
}
void main(void) {
    
    setup();
    Lcd_Write_String(MOTD_S);
    __delay_ms(1000);
    Lcd_Clear();
    PORTB = 0xF0;
    while(1);
    return;
}

void setup() {
    
    ANCON0 = 0;
    ANCON1 = 0;
    TRISD = 0b00001111;
    TRISE0 = 0;
    TRISE2 = 0;
    TRISB = 0b00000111;
    INTCON = 0b10010000;
    INTCON2 = 0b11111000;
    INTCON3 = 0b00111000;
    Lcd_Init();
}

void get_input() {
    __delay_ms(100);
    char input_char = 0;
    PORTB = 0x10; // Test Row A
    if (PORTBbits.RB0) {
        // Row A Col 1 
        input_char = '1';
    } else if (PORTBbits.RB1) {
        //ROW A Col 2
        input_char = '2';
    } else if (PORTBbits.RB2) {
        //ROW A Col 3
        input_char = '3';
    }
    
    PORTB = 0x20; // Test Row B
    if (PORTBbits.RB0) {
        // Row B Col 1 
        input_char = '4';
    } else if (PORTBbits.RB1) {
        //ROW B Col 2
        input_char = '5';
    } else if (PORTBbits.RB2) {
        //ROW B Col 3
        input_char = '6';
    }
    
    PORTB = 0x40; // Test Row C
    if (PORTBbits.RB0) {
        // Row C Col 1 
        input_char = '7';
    } else if (PORTBbits.RB1) {
        //ROW C Col 2
        input_char = '8';
    } else if (PORTBbits.RB2) {
        //ROW C Col 3
        input_char = '9';
    }
    
     PORTB = 0x80; // Test Row C
    if (PORTBbits.RB0) {
        // Row D Col 1 
        input_char = '*';
    } else if (PORTBbits.RB1) {
        //ROW D Col 2
        input_char = '0';
    } else if (PORTBbits.RB2) {
        //ROW D Col 3
        input_char = '#';
    }
      __delay_ms(100);
      Lcd_Clear();
      if (letters == 0 ) {
          Lcd_Write_String("checking password");
          letters = 5;
      }
      //else Lcd_Write_String( (char)letters);
      __delay_ms(300);
      Lcd_Clear();
      PORTB = 0xF0;
}
